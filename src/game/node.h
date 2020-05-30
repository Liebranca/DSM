//--- --- --- --- --- --- --- --- ---
//	* 	L 	I 	E 	B 	R 	A 	N 	C 	A 	*
//
//	-programmer, cg artist.
//	-contact: dantepaola25@gmail.com
//
//--- --- --- --- --- --- --- --- ---

#ifndef NODE_H
#define NODE_H

//--- --- --- --- --- --- --- --- ---
//	CRK BLOCK
//--- --- --- --- --- --- --- --- ---
//		0.	LIBRARIES
//	--- 	--- 	--- 	--- 	---
#include <vector>
#include "../backbone/reyn.h"
#include "../lymath/utils.h"
#include "../lymath/transform.h"
#include "../game/camera.h"

//		666.	DONE	.999 || ->DS
//--- END CRK BLOCK --- --- --- --- --- --- --- ---


//--- --- --- --- --- --- --- --- ---
//	CRK BLOCK
//--- --- --- --- --- --- --- --- ---

//	base game object class
//* std for nocols
//* used in celltree lookups
//TODO: use as base for lights
class DS_NODE
{

//		0.	CORE ATTRIBS & METHODS
//	--- 	--- 	--- 	--- 	---
	public:
		DS_NODE(const std::string& filename, bool isChild = false);
		virtual ~DS_NODE();

		inline glm::vec3& worldPosition() { return m->worldPosition(); }
		inline glm::quat& worldOrientation() { return m->worldOrientation(); }
		inline glm::vec3& worldScale() { return m->worldScale(); }
		
		void rotate(glm::quat twat) { m->rotate(twat); }
		void move(glm::vec3 mvec) { m->move(mvec); }
		
		glm::vec3 getFwdVec() { return this->bounds->faces[2].normal; }
		glm::vec3 getLftVec() { return this->bounds->faces[4].normal; }


//		1.	ROUTINES
//	--- 	--- 	--- 	--- 	---

		void onFrameStart() {
			this->visible = this->draw();
			this->obsDetected = false; this->readcomms();

			// MOVE TO DYNAMICS
			if (this->physMode == 2) {
				if (this->isOnGround()) {
					if (this->isJumping) {
						this->vel.y += fuck * 10.0f * this->speedmult;
						this->jumpTimer.restart();
						this->ground = nullptr;
					}
					//else { this->isOnGround = this->groundCheck(); }
				}
				else {
					if (!this->isJumping) {
						if (!(this->accel.y < 0)) { this->vel.y = fuck * (-10.0f); }
						else { this->vel.y -= fuck * (this->weight); }
						if (this->jumpTimer.curr) { this->jumpTimer.restart(); }
					}
					else {
						bool doSwap = jumpTimer.update(reyn->worldClock->getCurr());
						if (doSwap) {
							this->isJumping = false;
							this->vel.y = fuck * (-10.0f);
						}
						else { this->vel.y += fuck * 0.05f * this->speedmult; }
					}
				}
			}
		}

		void onFrameEnd() { ; }	

		void update() {

			// MOVE TO DYNAMICS
			if (this->canCol())
			{
				if (this->vel.x || this->vel.y || this->vel.z) {
					if (this->vel.x) { this->accel.x = this->vel.x; }
					if (this->vel.y) { this->accel.y = this->vel.y; }
					if (this->vel.z) { this->accel.z = this->vel.z; }
				}

				this->hasAccel = (this->accel.x || this->accel.y || this->accel.z);
				this->hasRot = (this->angvel.x || this->angvel.y || this->angvel.z);
				if (hasAccel || hasRot) {
					if (momentum < max_momentum) { momentum++; }
					if (this->hasAccel) {
						float maxspeed = fuck * 10.0f * this->speedmult;
						for (int ax = 0; ax < 3; ax++) {
							this->accel[ax] = lymath::clampf(this->accel[ax], -maxspeed, maxspeed);
							if (fabs(this->accel[ax]) <= gaol_physClamp) { this->accel[ax] = 0; }
							if (fabs(this->vel[ax]) <= gaol_physClamp) { this->vel[ax] = 0; }
						}
						this->move(this->accel); this->accel *= gaol_physDeacc;
						this->vel *= gaol_physDeacc;
					}
					if (this->hasRot) {
						for (int ax = 0; ax < 3; ax++) {
							if (fabs(this->angvel[ax]) <= gaol_physClamp) { this->angvel[ax] = 0; }
						}
						this->rotate({ 1, this->angvel }); this->angvel -= this->angvel * gaol_physDeacc;
					}
				}
				else { if (momentum > 0) { momentum--; } }
			}

		}

		//	standard drawing function
		//* frustum checks
		//* accounts for children
		//* accounts for render flag(s)
		bool draw() {
			this->model = this->m->getModel(); this->getBounds();
			if (this->doRender) {

				//	children (and thus grandchildren) get drawn first
				for (unsigned int i = 0; i < this->children.size(); i++) {
					this->children[i]->worldPosition() = this->mesh->po_xyz[i];
					this->children[i]->worldOrientation() = this->mesh->po_rot[i];
					this->children[i]->draw();
				}

				//	frustum check, then draw
				//	basis: we dont want children to dissapear when parent is not visible
				//	performance sacrifice to avoid objects popping in and out of existence
				if (active_camera->rectInFrustum(this->bounds->points)) {
					if (this->ansNode) { this->animate(); }
					glm::mat3 normal = this->m->getNormal();
					mesh->draw(this->model, normal);
					return true;
				}
			}
			return false;
		}

		//	alt drawing function
		//* no frustum check
		//* ignores children
		//* ignores render flag(s)
		//* direct call to meshs method
		void forceDraw() { mesh->draw(this->model, this->m->getNormal()); }


//		2.	PHYSICS
//	--- 	--- 	--- 	--- 	---
		
		// physics entry point
		//* performs safety checks
		//* triggers obstacle sim
		void physicsSim(DS_NODE* other, lymath::bf& groundCheck, bool& motCheck, bool& foundGround) {
			if (!other->bounds) { return; }
			if (other->canCol()) {
				if (this->distCheck(other)) {
					groundCheck = this->colCheckBox(other, motCheck);
					if (!foundGround) {
						if (groundCheck.a && !this->isJumping
							&& !(groundCheck.x > this->worldPosition().y + 0.5f)) {
							this->standingOn(other);
							this->clampToSurf();
						}
					}
				}

				//	janky AI pathfinding proto
				if (this->getTarget()) {
					if (this->getTarget() != other && !other->isMapNode() && !this->obsDetected) {
						this->obstacleSim(other);
					}
				}
			}
		}

		// issues faceTo then fwd command
		void moveToTarget() {
			glm::vec3 mvec = { 0,0,0 };
			if (this->faceTo()) {
				if (!this->distCheck(this->target, 0.5f)) {
					this->commands[0] = true;
				}
			}
		}

		// issues faceTo
		//* rots until facing
		//* already facing returns true
		bool faceTo(DS_NODE* newtarget = nullptr) {
			if (!newtarget) {
				if (!this->target) { return false; }
				newtarget = this->target;
			}
			return this->m->faceTo(newtarget->worldPosition());
		}

		// standard raycast
		//* colline == worldPos + vecFrom -> vecTo
		RAYDATA raytest(DS_NODE* other, glm::vec3 vecTo, glm::vec3 vecFrom = glm::vec3(0, 0, 0)) {
			if (!other->bounds) { return { false, {0,0,0} }; }
			return this->bounds->raytest(other->bounds, COLLINE(this->worldPosition() + vecFrom, vecTo));
		}

		// alt raycast
		//* colline == vecFrom -> vecTo
		RAYDATA raytest_alt(DS_NODE* other, glm::vec3 vecTo, glm::vec3 vecFrom = glm::vec3(0, 0, 0)) {
			if (!other->bounds) { return { false, {0,0,0} }; }
			return this->bounds->raytest(other->bounds, COLLINE(vecFrom, vecTo));
		}		

		// lazy sol flag for obstacle sim
		bool obsDetected = false;

		//	lazy sol obstacle sim
		//* relies too hard on raycasting
		//* does distance checks
		//* issues move commands
		bool obstacleSim(DS_NODE* other) {
			glm::vec3 vecTo = glm::vec3(0, 0.5f, 0) + this->worldPosition();
			RAYDATA ray = this->raytest(other, vecTo + this->getFwdVec() * 10.0f, { 0, 0.5f, 0 });
			if (!ray.hitObject) {
				ray = this->raytest(other, vecTo + this->getFwdVec() * 10.0f
					+ this->getLftVec() * 0.5f, { 0, 0.5f, 0 });
				if (!ray.hitObject) {
					ray = this->raytest(other, vecTo + this->getFwdVec() * 10.0f
						- this->getLftVec() * 0.5f, { 0, 0.5f, 0 });
				}
			}
			if (ray.hitObject) {
				this->obsDetected = true;
				glm::vec3 p1 = ray.point + this->getLftVec();
				glm::vec3 p2 = ray.point - this->getLftVec();
				
				if (glm::distance(this->target->worldPosition(), p1)
					> glm::distance(this->target->worldPosition(), p2)) {
						this->commands[2] = true;
					}
				else {
					this->commands[3] = true;
				}
			}
			return ray.hitObject;
		}

		//	usually a pre-col test
		//* checks bounds are close enough to touch
		//* fac can be unreliable
		bool distCheck(DS_NODE* other, float fac = 1.25f) {
			float l1 = other->bounds->area[0] + this->bounds->area[0], l2 = 0.0f;
			float yd = lymath::codist(this->worldPosition().y, other->worldPosition().y);
			bool e1 = glm::distance(lymath::xzvec(other->worldPosition()),
				lymath::xzvec(this->worldPosition())) < l1/fac;

			if (this->worldPosition().y < other->worldPosition().y) {
				l2 = this->bounds->area[1];
			}
			else {
				l2 = other->bounds->area[1];
			}
			
			bool e2 = yd < l2;

			return e1 && e2;
		}
		
		//	force nail to ground
		//* uses topface centroid of platform bounds
		void clampToSurf() {
			this->worldPosition().y = this->ground->bounds->faces[1].centroid.y;
		}

		glm::vec3 vel = glm::vec3(0, 0, 0);
		
		// physics workhorse
		//* performs check through selfProjection (needs improving for hivel)
		//* experimental angvel support
		//* ignores stoodon for y-motion
		//* groundchecks statics
		lymath::bf colCheckBox(DS_NODE* other, bool& mote) {

			glm::vec3 dirn = this->accel * 10.0f;

			COLBOX proj = this->bounds->selfProjection(dirn);
			glm::vec3 n = proj.boxToBox(other->bounds, glm::normalize(dirn));						

			if (n.x || n.y || n.z) {
				n = n * fuck * 5.5f;
				bool iang[] = { true, true, true };
				
				if (this->getColType(other)) { iang[0] = false; iang[1] = false; iang[2] = false; }

				if (n.x) {
					mote = false;
					if (this->accel.x) { this->vel.x = n.x; }
					if (!iang[2]) { this->angvel.z *= -1; iang[2] = true; }
					if (!iang[1]) { this->angvel.y *= -1; iang[1] = true; }
					other->vel.x = -n.x * other->weight;
				}
				if (n.y) {
					if (this->accel.y && other != this->ground) { this->vel.y = n.y; }
					if (!iang[0]) { this->angvel.x *= -1; iang[0] = true; }
					if (!iang[2]) { this->angvel.z *= -1; iang[2] = true; }
					other->vel.y = -n.y * other->weight;
				}
				if (n.z) {
					mote = false;
					if (this->accel.z) { this->vel.z = n.z; }
					if (!iang[0]) { this->angvel.x *= -1; iang[0] = true; }
					if (!iang[1]) { this->angvel.y *= -1; iang[1] = true; }
					other->vel.z = -n.z * other->weight;
				}
				if (other->physMode == 1) {
					return proj.groundCheck(other->bounds, -10.0f);
				}
			}
			
			return { false, 0.0f };

		}

		bool groundCheck() {
			return this->bounds->groundCheck(this->ground->bounds, -10.0f).a;
		}
		bool dynOnDyn(DS_NODE* other) {
			return other->physMode == 2 && this->physMode == 2;
		}
		bool getColType(DS_NODE* other) {
			if (other->physMode == 1 && this->physMode == 1) { return true; } return false;
		}
		bool canCol() {	return (this->physMode == 1 || this->physMode == 2); }
		
		float getSpeed() {
			return glm::length(this->accel + this->angvel) / (fuck * 2.0f);
		}
		bool pointInside(glm::vec3 point) {
			return this->bounds->pointInside(point);
		}
		void standingOn(DS_NODE* ground) { this->ground = ground; }
		

//		3.	ANIMATION
//	--- 	--- 	--- 	--- 	---
		bool usable = false, isOpenDoor = false;
		void animate() {
			if (!cAnim) { cAnim = this->ansTree->getAction(cAnim_id); }

			if (cAnim) {
				bool doSwap = this->ic.update(reyn->worldClock->getCurr());
				if (doSwap) {
					if (this->cAnim_id == "use") {
						int dirn = 1; if (!this->staticActive_toggle) { dirn = -1; }
						if (dirn == 1) {
							if (cFrame < this->cAnim->framerange - 1) { cFrame++; }
							if (this->usable) { this->isOpenDoor = true; }
							this->mesh = this->cAnim->frames[cFrame];
						}
						else {
							if (cFrame > 0) { cFrame--; }
							if (this->usable) { this->isOpenDoor = false; }
							this->mesh = this->cAnim->frames[cFrame];
						}
					}
					else {
						if (cFrame < this->cAnim->framerange - 1) { cFrame++; }
						else { cFrame = 0; }
						this->mesh = this->cAnim->frames[cFrame];
					}
				}
			}
			else { std::cout << "Anim_id " + cAnim_id + " not found" << std::endl; }
		};
		void useToggle(bool antiseg = false) {
			this->staticActive_toggle = !this->staticActive_toggle;
			if (this->staticActive_twin && !antiseg) {
				this->staticActive_twin->useToggle(true);
			}

			this->setAnim("use");
		}


//		3.	AI
//	--- 	--- 	--- 	--- 	---

		DS_NODE* getTarget() { return this->target; }
		void setTarget(DS_NODE* other) { this->target = other; }
		void removeTarget() { this->target = nullptr; }

		void readcomms() { this->clearcomms(); }
		void clearcomms() {
			for (unsigned int i = 0; i < this->commands.size(); i++) {
				this->togcomm(i, false);
			}
		}
		void togcomm(int i, bool v = true) { this->commands[i] = v; }


//		3.	SPATIAL
//	--- 	--- 	--- 	--- 	---
		bool sharesSpaceWith(DS_NODE* other) {
			bool samenov = this->currnov[0] == other->currnov[0] && this->currnov[1] == other->currnov[1];
			bool sameenn = this->currenn[0] == other->currenn[0] && this->currenn[1] == other->currenn[1];
			return samenov && sameenn;
		}
		bool novCheck(int x, int z) {
			bool novchange = (this->currnov[0] != x || this->currnov[1] != z);
			return novchange;
		}
		bool ennCheck(int x, int z) {
			bool ennchange = (this->currenn[0] != x || this->currenn[1] != z);
			return ennchange;
		}
		std::vector<int> onNovChange(int x, int z) {
			std::vector<int> oldnov = { this->currnov[0], this->currnov[1] };
			this->setCurrnov(x, z); return oldnov;
		}
		std::vector<int> onEnnChange(int x, int z) {
			std::vector<int> oldenn = { this->currenn[0], this->currenn[1] };
			this->setCurrenn(x, z); return oldenn;
		}
				
		DS_NODE* getGround() { return this->ground; }

		// INTERACT FUNC
		void setTwins(DS_NODE* other) {
			this->staticActive_twin = other; other->staticActive_twin = this;
			this->usable = true; other->usable = true;
		}


//		4.	MISC GETTERS & SETTERS
//	--- 	--- 	--- 	--- 	---
		
		inline bool isMapNode() { return (this->nodeType == 1); }
		
		inline bool isMoving() { return (this->hasAccel || this->hasRot); }

		inline std::vector<bool>& getCommands() { return this->commands; }

		inline bool isOnGround() { this->ground != nullptr; }
		inline bool allowJump() { return this->isOnGround() && !this->isJumping; }

		//	IFUNC: return weight * physmods (momentum, gravity... )
		inline float weight_real() { 
			return this->weight * 1.0f;
		}

		inline DS_NODE* getInSight() { return this->in_sight; }
		inline void setInSight(DS_NODE* other) { this->in_sight = other; }
		inline void forgetInSight() { this->in_sight = nullptr; }

		inline glm::vec3& getAccel() { return this->accel; }

	protected:
		inline DS_TRANSFORM* getTransform() { return m; }

		glm::vec3 accel = { 0,0,0 }, angvel = { 0,0,0 };

		float speedmult = 10.0f; // global movement velocity multiplier

		//	object mass
		//* determines resistance to force
		//* zero weight results turns object static
		float weight = 1.0f;
		bool isJumping = false;

		DS_MESH* mesh = nullptr;
		DS_ANIMTREE* ansTree = nullptr;
		DS_ACTION* cAnim = nullptr;
		DS_TICKER ic = DS_TICKER(0.0389f);
		DS_TICKER jumpTimer = DS_TICKER(0.10f); //actor
		unsigned int cFrame = 0; bool visible = false, doRender = true;

		unsigned int physMode = 0;
		int momentum = 0, max_momentum = 200;

		bool staticActive_toggle = false; DS_NODE* staticActive_twin = nullptr; //switch
		bool hasAccel = false, hasRot = false;

		COLBOX* bounds;
		DS_TRANSFORM* m = nullptr;
		glm::mat4 model; DS_NODE* target = nullptr; DS_NODE* ground = nullptr;
		std::string cAnim_id = "def";
		std::vector<bool> commands = { false, false, false, false, false };

		DS_NODE* in_sight = nullptr; // ptr to under crosshair

		void cAnimCheck(std::string anim_id) {
			if (this->cAnim_id != anim_id) { this->setAnim(anim_id); }
		}
		void setAnim(std::string anim_id) {
			cAnim = nullptr; cAnim_id = anim_id;
		}

	private:
		
		void setParent(DS_TRANSFORM* other_m) { m->setParent(other_m); }

		//	determines internal behaviour
		//* 0: nocol
		//* 1: static
		//* 2: hafdyn
		//* 3: fuldyn
		//* 4: proj
		//* 5: doors
		//* 6: containers
		//* 7: actors
		unsigned int nodeType = 0;
		bool ansNode;

		std::vector<DS_BINDPOINT*> props;
		std::vector<DS_NODE*> children;

		int currnov[2] = { 0,0 };
		int currenn[2] = { 0,0 };
		void setCurrnov(int x, int z) { this->currnov[0] = x; this->currnov[1] = z; }
		void setCurrenn(int x, int z) { this->currenn[0] = x; this->currenn[1] = z; }
		

		void getBounds() { 
			if (this->bounds) { delete this->bounds; this->bounds = nullptr; }
			this->bounds = this->mesh->bounds->genBox(this->model);
		}
		glm::vec3 getImpactForce(DS_NODE* other) {
			float w = lymath::gmaxf(
				this->weight, other->weight) - lymath::gminf(this->weight, other->weight);

			return lymath::xzvec(other->worldPosition() - this->worldPosition()) * (1.0f + w) * fuck;

		}
		
};

//		666.	DONE	.999 || ->DS
//--- END CRK BLOCK --- --- --- --- --- --- --- ---

extern DS_NODE* og;
extern std::vector<DS_NODE*> scene_objects;

#endif // NODE_H
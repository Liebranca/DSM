//--- --- --- --- --- --- --- --- ---
//  *   L   I   E   B   R   A   N   C   A   *
//
//  -programmer, cg artist.
//  -contact: dantepaola25@gmail.com
//
//--- --- --- --- --- --- --- --- ---

#ifndef NODE_H
#define NODE_H

//--- --- --- --- --- --- --- --- ---
//  CRK BLOCK
//--- --- --- --- --- --- --- --- ---
//      0.  LIBRARIES
//  ---     ---     ---     ---     ---
#include <vector>
#include "../backbone/reyn.h"
#include "../lymath/utils.h"
#include "../lymath/transform.h"
#include "../game/camera.h"

//      666.    DONE    .999 || ->DS
//--- END CRK BLOCK --- --- --- --- --- --- --- ---


//--- --- --- --- --- --- --- --- ---
//  CRK BLOCK
//--- --- --- --- --- --- --- --- ---

//  base game object class
//* std for nocols
//* used in celltree lookups
//TODO: use as base for lights
class DS_NODE
{

//      0.  CORE ATTRIBS & METHODS
//  ---     ---     ---     ---     ---
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


//      1.  ROUTINES
//  ---     ---     ---     ---     ---

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

        //  alt drawing function
        //* no frustum check
        //* ignores children
        //* ignores render flag(s)
        //* direct call to meshs method
        void forceDraw() { mesh->draw(this->model, this->m->getNormal()); }


//      2.  PHYSICS
//  ---     ---     ---     ---     ---

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

        //  lazy sol obstacle sim
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

//      3.  ANIMATION
//  ---     ---     ---     ---     ---

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


//      3.  AI
//  ---     ---     ---     ---     ---

        void readcomms() { this->clearcomms(); }
        void clearcomms() {
            for (unsigned int i = 0; i < this->commands.size(); i++) {
                this->togcomm(i, false);
            }
        }
        void togcomm(int i, bool v = true) { this->commands[i] = v; }


//      3.  SPATIAL
//  ---     ---     ---     ---     ---

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

        // INTERACT FUNC
        void setTwins(DS_NODE* other) {
            this->staticActive_twin = other; other->staticActive_twin = this;
            this->usable = true; other->usable = true;
        }


//      4.  MISC GETTERS & SETTERS
//  ---     ---     ---     ---     ---

        inline std::vector<bool>& getCommands() { return this->commands; }

        //  IFUNC: return weight * physmods (momentum, gravity... )

        inline DS_NODE* getInSight() { return this->in_sight; }
        inline void setInSight(DS_NODE* other) { this->in_sight = other; }
        inline void forgetInSight() { this->in_sight = nullptr; }

    protected:

        //  object mass
        //* determines resistance to force
        //* zero weight results turns object static

        DS_ANIMTREE* ansTree = nullptr;
        DS_ACTION* cAnim = nullptr;
        DS_TICKER ic = DS_TICKER(0.0389f);
        DS_TICKER jumpTimer = DS_TICKER(0.10f); //actor
        unsigned int cFrame = 0;

        bool staticActive_toggle = false; DS_NODE* staticActive_twin = nullptr; //switch

        std::string cAnim_id = "def";;

        void cAnimCheck(std::string anim_id) {
            if (this->cAnim_id != anim_id) { this->setAnim(anim_id); }
        }
        void setAnim(std::string anim_id) {
            cAnim = nullptr; cAnim_id = anim_id;
        }

    private:
        
        void setParent(DS_TRANSFORM* other_m) { m->setParent(other_m); }

        //  determines internal behaviour
        //* 0: nocol
        //* 1: static
        //* 2: hafdyn
        //* 3: fuldyn
        //* 4: proj
        //* 5: doors
        //* 6: containers
        //* 7: actors

        std::vector<DS_BINDPOINT*> props;
        std::vector<DS_NODE*> children;

        int currnov[2] = { 0,0 };
        int currenn[2] = { 0,0 };
        void setCurrnov(int x, int z) { this->currnov[0] = x; this->currnov[1] = z; }
        void setCurrenn(int x, int z) { this->currenn[0] = x; this->currenn[1] = z; }

};

//      666.    DONE    .999 || ->DS
//--- END CRK BLOCK --- --- --- --- --- --- --- ---

extern DS_NODE* og;
extern std::vector<DS_NODE*> scene_objects;

#endif // NODE_H
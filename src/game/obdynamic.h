//--- --- --- --- --- --- --- --- ---
//	* 	L 	I 	E 	B 	R 	A 	N 	C 	A 	*
//
//	-programmer, cg artist.
//	-contact: dantepaola25@gmail.com
//
//--- --- --- --- --- --- --- --- ---

#ifndef OBDYNAMIC_H
#define OBDYNAMIC_H

//--- --- --- --- --- --- --- --- ---
//	CRK BLOCK
//--- --- --- --- --- --- --- --- ---
//		0.	LIBRARIES
//	--- 	--- 	--- 	--- 	---

#include "node.h"

//		666.	DONE	.999 || ->DS
//--- END CRK BLOCK --- --- --- --- --- --- --- ---


//--- --- --- --- --- --- --- --- ---
//	CRK BLOCK
//--- --- --- --- --- --- --- --- ---

// static game object class
//* walls, floors, collumns
class DS_DYNAMIC : public DS_NODE
{

//		0.	CORE ATTRIBS & METHODS
//	--- 	--- 	--- 	--- 	---

	public:


//		1.	ROUTINES
//	--- 	--- 	--- 	--- 	---



//		2.	PHYSICS
//	--- 	--- 	--- 	--- 	---

		// converts mvec to local space
		//* assigns directly to accel
		//* only for dynamics
		void localizePos(glm::vec3 mvec) {
			mvec.y = this->accel.y;
			this->accel = this->getTransform()->localizePos(
				mvec, this->getFwdVec(), this->getLftVec());
		}


//		3.	AI
//	--- 	--- 	--- 	--- 	---

		void readcomms() {			
			std::string newanim = "def";
			std::vector<bool>& comms = this->getCommands();
			if (comms[0] || comms[1] || comms[2] || comms[3]) {
				glm::vec3 mvec = { 0,this->accel.y,0 }; float uspeed = fuck * 2.0f * this->speedmult;
				if (comms[0] || comms[1]) {
					if (comms[0]) { mvec.z = uspeed; newanim = "walk"; }
					else { mvec.z = -uspeed; }
				}
				if (comms[2] || comms[3]) {
					if (comms[2]) { mvec.x = uspeed; }
					else { mvec.x = -uspeed; }
				}
				if (mvec.x || mvec.z) {
					if (mvec.x && mvec.z) { mvec.x /= 1.35f; mvec.z /= 1.35f; }
				}
				this->localizePos(mvec);
			}
			if (comms[4]) { this->isJumping = true; }

			this->cAnimCheck(newanim);
			DS_NODE::readcomms();

		}
};

//		666.	DONE	.999 || ->DS
//--- END CRK BLOCK --- --- --- --- --- --- --- ---

#endif // OBDYNAMIC_H

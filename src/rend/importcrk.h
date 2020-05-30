#ifndef IMPORTCRK_H
#define IMPORTCRK_H
#include <vector>
#include "mesh.h"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/quaternion.hpp"

class DS_ACTION {

	public:
		DS_ACTION() { ; }
		virtual ~DS_ACTION() { 
			for (unsigned int i = 0; i < this->framerange; i++) {
				delete this->frames[i];
			}
		}

		unsigned int framerange;
		std::string name;
		std::vector<DS_MESH*> frames;

};

class DS_ANIMTREE {
	
	public:
		DS_ANIMTREE(std::vector<DS_ACTION*> actions) { this->actions = actions; }
		virtual ~DS_ANIMTREE() { 
			for (unsigned int i = 0; i < this->actions.size(); i++) {
				delete this->actions[i];
			}
		}

		std::vector<DS_ACTION*> actions;

		DS_ACTION* getAction(std::string& x) {

			for (unsigned int i = 0; i < this->actions.size(); i++) {
				if (this->actions[i]->name == x) {
					return this->actions[i];
				}
			}
			return nullptr;
		}
};

class DS_BINDPOINT {
	
	public:
		DS_BINDPOINT(glm::vec3 pos, glm::quat rot, std::string defmesh) { 
			this->pos = pos; this->rot = rot; this->defmesh = defmesh;
		}

		virtual ~DS_BINDPOINT() { ; }

		glm::vec3 pos;
		glm::quat rot;
		std::string defmesh;
};

class CRKFILE {

public:
	CRKFILE(const std::string& filename, std::string meshName, bool hasAnsData,
		int physMode, float weight, float speedmult, bool isMap,
		std::vector<DS_BINDPOINT*> props) {

		this->filename = &filename;
		this->meshName = meshName;
		this->hasAnsData = hasAnsData;
		this->physMode = physMode;
		this->weight = weight;
		this->speedmult = speedmult;
		this->isMap = isMap;
		this->props = props;

	};

	virtual ~CRKFILE() { ; }

	const std::string* filename = nullptr;
	std::string meshName;
	bool hasAnsData = false, isMap = false;
	int physMode = 0;
	float speedmult = 1.0, weight = 0.0;
	std::vector<DS_BINDPOINT*> props;

};

void readans(const std::string& filename, DS_VERTEX_ARRAY* verts,
	std::vector<glm::vec3>* po_xyz, std::vector<glm::quat>* po_rot,
	std::vector<glm::vec4>* gaol, unsigned int propnum);

void readansHed(const std::string& filename, std::vector<DS_ACTION*>* ansvec,
	unsigned int* numActions);

CRKFILE* readcrk(const std::string& filename, DS_VERTEX_ARRAY* verts,
	std::vector<unsigned int>* inds, unsigned int* nv, unsigned int* ni,
	std::string* meshName, std::string* shaderName,
	std::string* diff, bool* isAnsNode, std::vector<DS_BINDPOINT*>* props,
	std::vector<glm::vec4>* gaol, unsigned int* physMode,
	float* weight, float* speedmult, bool* isMap,
	std::string modpath, std::string shdpath, std::string texpath);

#endif // IMPORTCRK_H

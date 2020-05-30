#ifndef REYN_H
#define REYN_H

#include <string>
#include <vector>
#include "../rend/importcrk.h"
#include "../rend/shader.h"
#include "../rend/texture.h"
#include "../lymath/time.h"
#include "../lymath/Gaoler.h"

class REYN {

	public:

		REYN() { ; }

		void init(std::string& root) {
			this->root = root;
			this->worldClock = new DS_TIME;
		}

		DS_TIME* worldClock = nullptr;
		glm::vec2 worldCoor = { 0,0 };
		std::string worldName = "cruelty";

		std::string root;
		bool rugPulled = false;
		glm::vec3 rugVec = { 0,0,0 };

		inline std::string datapath() { return root + "data\\"; }
		inline std::string modpath() { return datapath() + "models\\"; }
		inline std::string shdpath() { return datapath() + "shaders\\"; }
		inline std::string anspath() { return datapath() + "ans\\"; }
		inline std::string texpath() { return datapath() + "textures\\"; }

		std::vector<CRKFILE*> pCrack;
		std::vector<std::string> nCrack;
		std::vector<DS_MESH*> pMeshes;
		std::vector<std::string> nMeshes;
		std::vector<DS_SHADER*> pShaders;
		std::vector<std::string> nShaders;
		std::vector<DS_TEXTURE*> pTextures;
		std::vector<std::string> nTextures;
		std::vector<DS_ANIMTREE*> pAnimTrees;
		std::vector<std::string> nAnimTrees;

		int geteet(std::vector<std::string> vec, const std::string& x) {
			std::vector<std::string>::iterator pp = std::find(vec.begin(), vec.end(), x);
			if (pp != vec.end()) {
				return std::distance(vec.begin(), pp);
			}
			else {
				return -1;
			}
		}

		virtual ~REYN() {

			for (unsigned int i = 0; i < pTextures.size(); i++) {
				delete pTextures[i];
			}
			for (unsigned int i = 0; i < pShaders.size(); i++) {
				delete pShaders[i];
			}
			for (unsigned int i = 0; i < pMeshes.size(); i++) {
				delete pMeshes[i];
			}
			for (unsigned int i = 0; i < pCrack.size(); i++) {
				delete pCrack[i];
			}
			for (unsigned int i = 0; i < pAnimTrees.size(); i++) {
				delete pAnimTrees[i];
			}

		}

};

extern REYN* reyn;
extern float fuck;
extern unsigned int NUM_LIGHTS;
extern unsigned int MAX_LIGHTS;
#define fBy reyn->worldClock->fBy

#endif //REYN_H

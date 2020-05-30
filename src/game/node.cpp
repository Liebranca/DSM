#include "node.h"
#include <iostream>

DS_NODE* og = nullptr;
std::vector<DS_NODE*> scene_objects;

DS_NODE::DS_NODE(const std::string& filename, bool isChild) {

	this->m = new DS_TRANSFORM();

	int index = reyn->geteet(reyn->nCrack, filename);
	if (index < 0) {
		DS_VERTEX_ARRAY verts; std::vector<unsigned int> indices;
		unsigned int numVerts, numIndices; std::string meshName, shaderName, textureName;
		std::vector<glm::vec4> gaol;

		std::cout << "Load CRK " + filename + "... ";
		reyn->pCrack.push_back(readcrk(reyn->modpath()+filename+".crk", &verts, &indices, &numVerts,
			&numIndices, &meshName, &shaderName, &textureName, &this->ansNode,
			&this->props, &gaol, &this->physMode, &this->weight,
			&this->speedmult, &this->mapNode,
			reyn->modpath(), reyn->shdpath(), reyn->texpath()));
		
		reyn->nCrack.push_back(filename);

		index = reyn->geteet(reyn->nMeshes, filename);
		if (index < 0) {
			DS_BOUNDS* bounds = new DS_BOUNDS(gaol);
			this->mesh = new DS_MESH(verts, numVerts, indices, numIndices, meshName, bounds);
			reyn->nMeshes.push_back(filename); reyn->pMeshes.push_back(this->mesh);
		}
		else { this->mesh = reyn->pMeshes[index]; }

		index = reyn->geteet(reyn->nShaders, reyn->shdpath() + shaderName);
		if (index < 0) {
			this->mesh->shader = new DS_SHADER(reyn->shdpath() + shaderName);
			reyn->nShaders.push_back(reyn->shdpath() + shaderName);
			reyn->pShaders.push_back(this->mesh->shader);
		}
		else { this->mesh->shader = reyn->pShaders[index]; }

		index = reyn->geteet(reyn->nTextures, reyn->texpath() + textureName);
		if (index < 0) {
			this->mesh->diffuse = new DS_TEXTURE(reyn->texpath() + textureName + ".joj");
			reyn->nTextures.push_back(reyn->texpath() + textureName);
			reyn->pTextures.push_back(this->mesh->diffuse);
		}
		else { this->mesh->diffuse = reyn->pTextures[index]; }

		std::cout << "done." << std::endl;
		for (unsigned int po_index = 0; po_index < this->props.size(); po_index++) {
			this->children.push_back(new DS_NODE(props[po_index]->defmesh, true));
			this->mesh->po_xyz.push_back(this->props[po_index]->pos);
			this->mesh->po_rot.push_back(this->props[po_index]->rot);
			this->children[po_index]->setParent(this->getTransform());
		}

		if (ansNode) {
			std::cout << "Unpacking ANS data for " + meshName + "... ";
			
			const std::string& ansfolder = reyn->anspath() + meshName + "\\";
			std::vector<DS_ACTION*> ansvec;
			unsigned int numActions = 0;

			readansHed(ansfolder + meshName + ".hed", &ansvec, &numActions);
			for (unsigned int i = 0; i < numActions; i++) {

				for (unsigned int a = 0; a < ansvec[i]->framerange; a++) {
					DS_VERTEX_ARRAY ansverts;
					std::vector<glm::vec3> po_xyz;
					std::vector<glm::quat> po_rot;
					std::vector<glm::vec4> gaol;

					readans(ansfolder + ansvec[i]->name + "\\frame" + std::to_string(a) + ".ans",
						&ansverts, &po_xyz, &po_rot, &gaol, this->props.size());

					ansverts.uv = verts.uv;
					DS_BOUNDS* bounds = new DS_BOUNDS(gaol);
					ansvec[i]->frames.push_back(new DS_MESH(ansverts, numVerts,
						indices, numIndices, meshName, bounds));

					ansvec[i]->frames[a]->shader = this->mesh->shader;
					ansvec[i]->frames[a]->diffuse = this->mesh->diffuse;
					ansvec[i]->frames[a]->po_xyz = po_xyz;
					ansvec[i]->frames[a]->po_rot = po_rot;
				}
			}

			this->ansTree = new DS_ANIMTREE(ansvec);
			reyn->pAnimTrees.push_back(this->ansTree); reyn->nAnimTrees.push_back(filename);
			std::cout << "done.\n";

		}
	}

	else { 
		CRKFILE* crack = reyn->pCrack[index];
		index = reyn->geteet(reyn->nMeshes, filename);
		this->mesh = reyn->pMeshes[index];
		this->physMode = crack->physMode;
		this->weight = crack->weight;
		this->speedmult = crack->speedmult;
		this->mapNode = crack->isMap;
		
		if (crack->hasAnsData) {
			index = reyn->geteet(reyn->nAnimTrees, filename);
			this->ansTree = reyn->pAnimTrees[index];
			this->ansNode = true;
		}

		this->props = crack->props;
		for (unsigned int po_index = 0; po_index < this->props.size(); po_index++) {
			this->children.push_back(new DS_NODE(this->props[po_index]->defmesh, true));
			this->children[po_index]->setParent(this->getTransform());
		}

	}

	if (!isChild) { scene_objects.push_back(this); }

};

DS_NODE::~DS_NODE() { 
	for (unsigned int i = 0; i < this->children.size(); i++) {
		delete this->children[i];
	}

}

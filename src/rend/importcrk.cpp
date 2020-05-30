#include <iostream>
#include <fstream>
#include <sstream>
#include "importcrk.h"
#include "glm/glm.hpp"


void readansHed(const std::string& filename, std::vector<DS_ACTION*>* ansvec,
	unsigned int* numActions) {
	
	std::ifstream file; std::string curr; unsigned int i = 0, a = 0;
	std::string line; std::string value;

	file.open(filename.c_str());

	if (!file) { std::cerr << "Unable to open file " << filename << std::endl; exit(1); }
	while (std::getline(file, line))
	{
		std::stringstream _line(line);
		DS_ACTION* act = new DS_ACTION();
		a = 0; ansvec->push_back(act);
		while (std::getline(_line, value, ' '))
		{
			if (a == 0) { ansvec->at(i)->name = value; }
			else { ansvec->at(i)->framerange = stoi(value); }
			a++;
		}
		i++;
	}

	*numActions = i;
	file.close();
}

void readans(const std::string& filename, DS_VERTEX_ARRAY* verts,
	std::vector<glm::vec3>* po_xyz, std::vector<glm::quat>* po_rot,
	std::vector<glm::vec4>* gaol, unsigned int propnum) {

	unsigned int iStart = 0, iEnd = 48;
	std::vector<unsigned char> file = lybyte::frombin(filename);

	for (unsigned int i = iStart; i < iEnd; i += 6) {
		gaol->push_back(glm::vec4(
			lybyte::cfloat2(i),
			lybyte::cfloat2(i + 2),
			lybyte::cfloat2(i + 4),
			1
		));
	}

	iStart = iEnd; iEnd = iStart + 14 * propnum;
	glm::vec3 xyz(0, 0, 0); glm::quat rot(0, 0, 0, 1);
	for (unsigned int i = iStart; i < iEnd; i += 14) {
		xyz[0] = lybyte::cfloat2(i);
		xyz[1] = lybyte::cfloat2(i + 2);
		xyz[2] = lybyte::cfloat2(i + 4);
		po_xyz->push_back(xyz);

		rot[0] = lybyte::cfloat2(i + 6);
		rot[1] = lybyte::cfloat2(i + 8);
		rot[2] = lybyte::cfloat2(i + 10);
		rot[3] = lybyte::cfloat2(i + 12);
		po_rot->push_back(rot);
	}

	iStart = iEnd; iEnd = file.size();
	glm::vec3 normal(0, 0, 0);
	for (unsigned int i = iStart; i < iEnd; i += 12) {
		verts->xyz.push_back(glm::vec3(
			lybyte::cfloat2(i),
			lybyte::cfloat2(i + 2),
			lybyte::cfloat2(i + 4)
			));

		verts->normal.push_back(glm::vec3(
			lybyte::cfloat2(i + 6),
			lybyte::cfloat2(i + 8),
			lybyte::cfloat2(i + 10)
		));
	}

}

CRKFILE* readcrk(const std::string& filename, DS_VERTEX_ARRAY* verts,
	std::vector<unsigned int>* indices, unsigned int* nv,
	unsigned int* ni, std::string* meshName, std::string* shaderName,
	std::string* diff, bool* isAnsNode, std::vector<DS_BINDPOINT*>* props,
	std::vector<glm::vec4>* gaol, unsigned int* physMode, float* weight,
	float* speedmult, bool* isMap,
	std::string modpath, std::string shdpath, std::string texpath)
{
	unsigned int hedsize, hasprops, iStart, iEnd;
	std::vector<unsigned char> file = lybyte::frombin(filename);
	
	hedsize = lybyte::cuint2(0);
	hasprops = lybyte::cuint2(2);

	*nv = lybyte::cuint2(4);
	*ni = lybyte::cuint2(6);

	std::vector<bool> flags = lybyte::tobits(file[8], 8);
	*isAnsNode = flags[0]; *isMap = flags[1];
	*physMode = lybyte::todecimal({ flags[2], flags[3] });

	*weight = lybyte::cfloat2(9);
	*speedmult = lybyte::cfloat2(11);

	iStart = 13; iEnd = iStart + 48;
	for (unsigned int i = iStart; i < iEnd; i += 6) {		
		gaol->push_back(glm::vec4(
			lybyte::cfloat2(i),
			lybyte::cfloat2(i + 2),
			lybyte::cfloat2(i + 4),
			1
			));
	}

	iStart = iEnd; iEnd = iStart + hasprops;
	glm::vec3 po_xyz(0, 0, 0); glm::quat po_rot(0, 0, 0, 1);
	unsigned int propId; std::string propname;
	for (unsigned int i = iStart; i < iEnd; i += 17) {
		propId = lybyte::cuint3(i);
		propname = lybyte::cpath_fromreg(propId, modpath);

		po_xyz[0] = lybyte::cfloat2(i + 3);
		po_xyz[1] = lybyte::cfloat2(i + 5);
		po_xyz[2] = lybyte::cfloat2(i + 7);

		po_rot[0] = lybyte::cfloat2(i + 9);
		po_rot[1] = lybyte::cfloat2(i + 11);
		po_rot[2] = lybyte::cfloat2(i + 13);
		po_rot[3] = lybyte::cfloat2(i + 15);

		props->push_back(new DS_BINDPOINT(po_xyz, po_rot, propname));			
	}

	*meshName = filename;
	while (meshName->find('\\') != meshName->npos) {
		meshName->erase(0, meshName->find('\\') + 1);
	}

	meshName->erase(meshName->rfind("."), meshName->length());
	iStart = iEnd; iEnd = hedsize;
	*shaderName = lybyte::cpath_fromreg(file[iStart], shdpath);
	*diff = lybyte::cpath_fromreg(lybyte::cuint2(hedsize-2), texpath);

	iStart = iEnd; iEnd = iStart + (*nv) * 16;
	for (unsigned int i = iStart; i < iEnd; i += 16) {
		
		verts->xyz.push_back(glm::vec3(
			lybyte::cfloat2(i),
			lybyte::cfloat2(i + 2),
			lybyte::cfloat2(i + 4)
			));

		verts->normal.push_back(glm::vec3(
			lybyte::cfloat2(i + 6),
			lybyte::cfloat2(i + 8),
			lybyte::cfloat2(i + 10)
			));

		verts->uv.push_back(glm::vec2(
			lybyte::cfloat2(i + 12),
			lybyte::cfloat2(i + 14)
			));

	}

	iStart = iEnd; iEnd = file.size();
	for (unsigned int i = iStart; i < iEnd; i += 6) {
		indices->push_back(lybyte::cuint2(i));
		indices->push_back(lybyte::cuint2(i + 2));
		indices->push_back(lybyte::cuint2(i + 4));
	}

	lybyte::cfile_close();
	return new CRKFILE(filename, *meshName, *isAnsNode, *physMode, *weight, *speedmult, *isMap, *props);

};

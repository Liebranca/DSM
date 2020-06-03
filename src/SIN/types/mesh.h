#ifndef SIN_MESH_H
#define SIN_MESH_H

#include "..\SIN.h"
#include "material.h"

//	- --- - --- - --- - --- -

class Mesh {
public:
	Mesh(VertexPacked verts[], uint indices[], char name[]);

	~Mesh();

//	- --- - --- - --- - --- -

private:
	uint drawmode, drawcount, numverts, id;
	uint bufferPosition; size_t bufferOffset;

	std::vector<VertexPacked> verts_raw;
	std::vector<uint> indices_raw;

	Material* material;

	// Also add pointer to batch buffer?

};

#endif // SIN_MESH_H

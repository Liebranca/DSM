#include "mesh.h"

//	- --- - --- - --- - --- -

constexpr char signature[] = { 	0x46, 0x43, 0x4b, 0x42,
								0x21, 0x54, 0x43, 0x48,
								0x45, 0x53,	0x47, 0x45,
								0x54, 0x24, 0x24, 0x24	};

Mesh::Mesh(VertexPacked verts[], uint indices[], char name[]) {
	
	drawcount = sizeof(indices) / sizeof(indices[0]);
	numverts = sizeof(verts) / sizeof(verts[0]);
	
	verts_raw.resize(numverts);
	for (uint i = 0; i < numverts; i++) {
		verts_raw[i] = verts[i];
	}

	indices_raw.resize(drawcount);
	/*for (uint i = 0; i < drawcount; i++) {
		indices_raw[i] = indices[i];
	}*/

	drawmode = 0;

}

//	- --- - --- - --- - --- -

Mesh::~Mesh() {
	/*for (uint i = 0; i < vbos.size(); i++) {
		glDeleteBuffers(1, &vbos[i]);
	}
	glDeleteVertexArrays(1, &vao);*/
}


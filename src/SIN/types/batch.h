#ifndef SIN_VBO_H
#define SIN_VBO_H

//	- --- - --- - --- - --- -

#include "..\SIN.h"
#include "vertex.h"

namespace SIN {

	const uint batch_maxMeshes = 64;
	const uint batch_maxVerts = 16384;

	const size_t batch_vboSize = batch_maxVerts * sizeof(Vertex);
	const size_t batch_iboSize = batch_maxVerts * sizeof(uint);

	const size_t batch_uboSize = 4 * 4; // placeholder for per-sector light info
}

//	- --- - --- - --- - --- -

// Baseline vertex buffer class
	class BatchBuffer {
	public:
		BatchBuffer(uint usageflag = 0, bool genUbo = false);
		~BatchBuffer();

//	- --- - --- - --- - --- -

	private:
		enum { VBO, IBO, UBO, NUM_BUFFERS };
		uint vao_location;
		uint buffs[NUM_BUFFERS];

};

#endif // SIN_VBO_H

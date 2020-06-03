#ifndef SIN_VERTEX_H
#define SIN_VERTEX_H

#include "../SIN.h"

//	- --- - --- - --- - --- -

struct VertexPacked { float2 co[3]; float2 n[3]; float2 uv[2]; };
typedef std::vector<VertexPacked> VertexPacked_array;

struct Vertex { float co[3]; float n[3]; float uv[3]; };

#endif // SIN_VERTEX_H

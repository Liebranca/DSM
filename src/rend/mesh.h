#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include "shader.h"
#include "texture.h"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/quaternion.hpp"
#include "GL/glew.h"
#include "../lymath/Gaoler.h"

class DS_VERTEX_ARRAY
{
	public:
		DS_VERTEX_ARRAY() {};
		virtual ~DS_VERTEX_ARRAY() {};

		std::vector<glm::vec3> xyz;
		std::vector<glm::vec3> normal;
		std::vector<glm::vec2> uv;

};

class DS_MESH
{
	public:
		DS_MESH() { ; }
		DS_MESH(DS_VERTEX_ARRAY verts, unsigned int numVerts,
			std::vector<unsigned int> indices, unsigned int numIndices,
			std::string _name, DS_BOUNDS* bounds);

		virtual ~DS_MESH();
		
		void draw(glm::mat4& model, glm::mat3 normal);
		void depthDraw();

		DS_SHADER* shader;
		DS_TEXTURE* diffuse;
		DS_BOUNDS* bounds;

		std::vector<glm::vec3> po_xyz;
		std::vector<glm::quat> po_rot;

		std::string name;

	private:
		enum {POSITION_VB, NORMAL_VB, TEXCOORD_VB, INDEX_VB, NUM_BUFFERS};

		unsigned int drawCount, id;
		GLuint vertArray;
		GLuint vertBuffers[NUM_BUFFERS];

};

#endif // MESH_H
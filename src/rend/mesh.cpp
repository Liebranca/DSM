#include "mesh.h"
#include <iostream>
#include <vector>

DS_MESH::DS_MESH(DS_VERTEX_ARRAY verts, unsigned int numVerts,
	std::vector<unsigned int> indices, unsigned int numIndices,
	std::string _name, DS_BOUNDS* bounds)
{
	this->bounds = bounds;
	drawCount = numIndices;
	name = _name;

	glGenVertexArrays(1, &vertArray);
	glBindVertexArray(vertArray);

	glGenBuffers(NUM_BUFFERS, vertBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, vertBuffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, numVerts * sizeof(glm::vec3),
		&verts.xyz[0], GL_STATIC_DRAW);
			
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, numVerts * sizeof(glm::vec3),
		&verts.normal[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertBuffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, numVerts * sizeof(glm::vec2),
		&verts.uv[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertBuffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	numIndices * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

}

DS_MESH::~DS_MESH()
{
	delete this->bounds;

	glDeleteBuffers(1, &vertBuffers[POSITION_VB]);
	glDeleteBuffers(1, &vertBuffers[NORMAL_VB]);
	glDeleteBuffers(1, &vertBuffers[TEXCOORD_VB]);

	glDeleteBuffers(1, &vertBuffers[INDEX_VB]);
	glDeleteVertexArrays(1, &vertArray);

}

void DS_MESH::depthDraw() {

	glBindVertexArray(vertArray);
	glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void DS_MESH::draw(glm::mat4& model, glm::mat3 normal)
{
	shader->bind();
	shader->update(model, normal);
	diffuse->bind(0);

	glBindVertexArray(vertArray);
	glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

}

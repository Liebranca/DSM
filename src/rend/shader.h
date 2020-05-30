#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <string>
#include <vector>
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "../game/camera.h"

class DS_SHADER
{
	public:
		DS_SHADER() { ; }
		DS_SHADER(const std::string& filename);

		virtual ~DS_SHADER();
		void bind();

		void update(glm::mat4& model, glm::mat3 normal);
		void depthUpdate(glm::mat4 model);

		void setUniform4fv(std::string name, glm::mat4 mat);
		unsigned int getULoc(std::string name);

	private:
		static const unsigned int NUM_SHADERS = 2;
		enum { TRANSFORM_U, NORMAL_U, EYE_U, GLIGHT_U, SKY_U, NUM_UNIFORMS };

		GLuint program;
		GLuint shaders[NUM_SHADERS];
		GLuint uniforms[NUM_UNIFORMS];

};

#endif // SHADER_H
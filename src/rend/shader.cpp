#include "shader.h"
#include "Light.h"
#include <fstream>

static std::string loadShader(const std::string& fileName);
static void checkShaderError(GLuint shader, GLuint flag,
bool isProgram, const std::string& errorMessage);

static GLuint createShader(const std::string& text, GLenum shaderType);

DS_SHADER::DS_SHADER(const std::string& filename)
{

	program = glCreateProgram();
	shaders[0] = createShader(loadShader(filename + ".vs"), GL_VERTEX_SHADER);
	shaders[1] = createShader(loadShader(filename + ".fs"), GL_FRAGMENT_SHADER);

	for (unsigned int i = 0; i < NUM_SHADERS; i++)
		glAttachShader(program, shaders[i]);
	
	glBindAttribLocation(program, 0, "POSITION");
	glBindAttribLocation(program, 1, "NORMAL");
	glBindAttribLocation(program, 2, "UV");

	glLinkProgram(program);
	checkShaderError(program, GL_LINK_STATUS, true, "SHADER LINKING DONE FUKK'D UP");
	glValidateProgram(program);
	checkShaderError(program, GL_VALIDATE_STATUS, true, "SHADER VALIDATION DONE FUKK'D UP");

	uniforms[TRANSFORM_U] = glGetUniformLocation(program, "TRANSFORM");
	uniforms[NORMAL_U] = glGetUniformLocation(program, "nMAT");
	uniforms[EYE_U] = glGetUniformLocation(program, "EYE");
	uniforms[GLIGHT_U] = glGetUniformLocation(program, "GLIGHT");
	uniforms[SKY_U] = glGetUniformLocation(program, "skyColor");

};

DS_SHADER::~DS_SHADER()
{
	for (unsigned int i = 0; i < NUM_SHADERS; i++)
	{
		glDetachShader(program, shaders[i]);
		glDeleteShader(shaders[i]);
	}

	glDeleteProgram(program);
};

void DS_SHADER::setUniform4fv(std::string name, glm::mat4 mat) {
	glUniformMatrix4fv(this->getULoc(name), 1, GL_FALSE, &mat[0][0]);
}

unsigned int DS_SHADER::getULoc(std::string name) {
	return glGetUniformLocation(this->program, name.c_str());
}

void DS_SHADER::bind()
{ 
	glUseProgram(program);
};

void DS_SHADER::depthUpdate(glm::mat4 model) {
	glUniformMatrix4fv(uniforms[TRANSFORM_U], 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(uniforms[EYE_U], 1, GL_FALSE, &actcam_viewproj[0][0]);
}

void DS_SHADER::update(glm::mat4& model, glm::mat3 normal)
{
	glUniformMatrix4fv(uniforms[TRANSFORM_U], 1, GL_FALSE, &model[0][0]);
	glUniformMatrix3fv(uniforms[NORMAL_U], 1, GL_FALSE, &normal[0][0]);
	glUniformMatrix4fv(uniforms[EYE_U], 1, GL_FALSE, &actcam_viewproj[0][0]);
	glUniform3fv(uniforms[GLIGHT_U], 1, &actcam_fwd[0]);
	glUniform4fv(uniforms[SKY_U], 1, &skyColor[0]);

	GLint loc;
	loc = glGetUniformLocation(program, "NUM_LIGHTS");
	glUniform1ui(loc, nearbyLights.size());

	std::string li;
	DS_LIGHT* clight = nullptr;
	for (unsigned int i = 0; i < MAX_LIGHTS; i++) {
		if (i == NUM_LIGHTS || i == nearbyLights.size()) { break; }
		
		clight = nearbyLights[i];
		li = std::to_string(i);
		loc = glGetUniformLocation(program, ("lights["+li+"].position").c_str());
		glUniform4fv(loc, 1, &clight->shedPosition()[0]);
		loc = glGetUniformLocation(program, ("lights[" + li + "].color").c_str());
		glUniform4fv(loc, 1, &clight->getCol()[0]);
		loc = glGetUniformLocation(program, ("lights[" + li + "].radius").c_str());
		glUniform1f(loc, clight->radius);
	}

}

static GLuint createShader(const std::string& text, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);
	if (!shader)
		std::cerr << "SHADER CREATION DONE FUKK'D UP" << std::endl;

	const GLchar* shaderSource[1];
	GLint shaderSourceLen[1];
	
	shaderSource[0] = text.c_str();
	shaderSourceLen[0] = text.length();

	glShaderSource(shader, 1, shaderSource, shaderSourceLen);
	glCompileShader(shader);
	
	checkShaderError(shader, GL_COMPILE_STATUS, false, "SHADER COMPILING DONE FUKK'D UP");	 

	return shader;

}

static std::string loadShader(const std::string& fileName)
{
	std::ifstream file;
	file.open((fileName).c_str());

	std::string output;
	std::string line;

	if (file.is_open())
		while (file.good())
		{
			getline(file, line);
			output.append(line + "\n");
		}

	else
		std::cerr << "SHADER " << fileName << " DONE FUKK'D UP" << std::endl;

	return output;

}

static void checkShaderError(GLuint shader, GLuint flag,
bool isProgram, const std::string& errorMessage)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };
	if (isProgram)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader, flag, &success);

	if (success == GL_FALSE)
	{
		if (isProgram)
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		else
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);

		std::cerr << errorMessage << ": " << error << std::endl;
	}

}
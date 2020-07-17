#include "shader.h"
#include "Light.h"
#include <fstream>

void __LIGHTBLOCK(glm::mat4& model, glm::mat3 normal)
{

    /* TODO: move this light shoving-up-the-uniform to SIN

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
    }*/

}

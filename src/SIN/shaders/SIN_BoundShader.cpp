#include "SIN_BoundShader.h"

const GLchar* SIN_BoundShader_source_v[1] = 
{
R"glsl(#version 150

in       vec3  Position;
uniform  mat4  ViewProjection;

void main()                                     { gl_Position = ViewProjection * vec4(Position, 1.0);                   }
)glsl"
};

const GLchar* SIN_BoundShader_source_p[1] =
{
R"glsl(#version 150

uniform vec3 CamPos;

void main()                                     { gl_FragColor = vec4(CamPos, 1);                                       }
)glsl"
};
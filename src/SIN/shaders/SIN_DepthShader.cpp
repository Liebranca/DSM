#include "SIN_DepthShader.h"

const GLchar* SIN_DepthShader_source_v[1] = 
{
R"glsl(#version 150

in       vec3  Position;

uniform  mat4  Model;
uniform  mat4  ViewProjection;

void main()                                     { vec4 worldPosition = Model * vec4(Position, 1.0);
                                                  gl_Position        = ViewProjection * worldPosition;                  }
)glsl"
};

const GLchar* SIN_DepthShader_source_p[1] =
{
R"glsl(#version 150

out float fragdepth;

void main()                                     { fragdepth = gl_FragCoord.z;                                           }
)glsl"
};
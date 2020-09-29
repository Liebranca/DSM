#include "SIN_CanvasShader.h"

const GLchar* SIN_CanvasShader_source_v[1] = 
{
R"glsl(#version 150

in       vec3  Position;
in       vec2  UV;

out      vec2  texCoords;

void main()                                     { gl_Position = vec4(Position.x, Position.y, 0, 1.0);
                                                  texCoords   = UV;                                                     }
)glsl"
};

const GLchar* SIN_CanvasShader_source_p[1] =
{
R"glsl(#version 150

in vec2 texCoords;
uniform sampler2D DiffuseMap;

float zNear = 0.001;
float zFar  = 100.0;

float LinearizeDepth(float depth)
{
    float z = (depth * 2.0) - 1.0;
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
}

float calcShadow()
{

    float closestDepth = LinearizeDepth(texture2D(DiffuseMap, texCoords).r);

    return clamp(closestDepth, 0.0, 1.0);
}

void main()                                     { float depth = calcShadow();
                                                  if(depth > 0.95) { discard; }
    gl_FragColor = vec4(vec3(0), 1);                           }
)glsl"
};
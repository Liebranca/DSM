#include "SIN_CanvasShader.h"
#include "SIN_ShaderBlocks_DefHed.h"

const GLchar* SIN_CanvasShader_source_v[2] = 
{

SIN_ShaderBlock_DefHed,

R"glsl(

in       vec3  Position;
in       vec2  UV;

out      vec2  texCoords;

void main()                                     { gl_Position = vec4(Position.x, Position.y, 0, 1.0);
                                                  texCoords   = UV;                                                     }
)glsl"
};

const GLchar* SIN_CanvasShader_source_f[2] =
{

SIN_ShaderBlock_DefHed,

R"glsl(

in vec2 texCoords;
uniform sampler2DArray Surface;

void main()
{
    vec3 color   = texture(Surface, vec3(texCoords, 0)).rgb;
    gl_FragColor = vec4(color, 1);
}

)glsl"
};

const shaderParams SIN_CanvasShader =
{

    SIN_CanvasShader_source_v,                      // Vertex sources
    SIN_CanvasShader_source_f,                      // Fragment sources

    { "Position", "UV"                      },      // Attributes

    {                                       },      // Uniforms

    {                                       },      // UBOs

    { "Surface"                             },      // Samplers

    2,                                              // Number of vertex shader blocks
    2,                                              // Number of fragment shader blocks
    2,                                              // Number of attributes
    0,                                              // Number of uniforms
    0,                                              // Number of UBOs
    1,                                              // Number of samplers

    0                                               // Flags

};

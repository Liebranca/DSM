#include "SIN_DepthShader.h"
#include "SIN_ShaderBlocks_DefHed.h"
#include "SIN_ShaderBlocks_Light.h"

const char* SIN_DepthShader_source_v[3] =
{

SIN_ShaderBlock_DefHed,
SIN_ShaderBlock_VertLightIn,

R"glsl(

in      vec3 Position;

uniform mat4 Model;
uniform int  LightID;

void main()
{
    vec4 worldPosition = Model * vec4(Position, 1.0);

    gl_Layer           = LightID;
    gl_Position        = LightIn.lamps[LightID].mat * worldPosition;
}

)glsl"
};

const char* SIN_DepthShader_source_f[2] =
{

SIN_ShaderBlock_DefHed,

R"glsl(

out float FragDepth;

void main()
{
    FragDepth = gl_FragCoord.w;
}

)glsl"
};

const shaderParams SIN_DepthShader =
{

    SIN_DepthShader_source_v,                       // Vertex sources
    SIN_DepthShader_source_f,                       // Fragment sources

    { "Position"                            },      // Attributes

    { "Model", "LightID"                    },      // Uniforms

    { SIN_ShaderBlock_LightUniforms[0]      },      // UBOs

    {                                       },      // Samplers

    3,                                              // Number of vertex shader blocks
    2,                                              // Number of fragment shader blocks
    1,                                              // Number of attributes
    2,                                              // Number of uniforms
    1,                                              // Number of UBOs
    0,                                              // Number of samplers

    0                                               // Flags

};

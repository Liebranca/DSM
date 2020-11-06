#include "SIN_ShaderBlocks_Ambient.h"

const GLchar* SIN_ShaderBlock_AmbientUniforms[1] = 
{
    "Ambient"
};

const GLchar* SIN_ShaderBlock_VertAmbient =

R"glsl(

layout (std140) uniform Ambient {

    mat4 ViewProjection;
    vec4 CamFwd;
    vec4 Color;
} AmbientIn;

out vec3 CamFwd;
out vec4 AmbientColor;

void SIN_setAmbient()
{
    CamFwd       = AmbientIn.CamFwd.xyz;
    AmbientColor = AmbientIn.Color;
}

)glsl"
;

const GLchar* SIN_ShaderBlock_FragAmbient =

R"glsl(

in vec3 CamFwd;
in vec4 AmbientColor;

)glsl"
;

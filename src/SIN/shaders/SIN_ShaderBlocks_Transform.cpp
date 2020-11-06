#include "SIN_ShaderBlocks_Transform.h"

const GLchar* SIN_ShaderBlock_TransformAttribs[5] = 
{
    "Position",
    "Normal",
    "Tangent",
    "Bitangent",
    "UV"
};

const GLchar* SIN_ShaderBlock_TransformUniforms[2] =
{
    "Model",
    "ModelInverseTranspose"
};

const GLchar* SIN_ShaderBlock_VertTransform = 

R"glsl(

in      vec3  Position;
in      vec3  Normal;
in      vec3  Tangent;
in      vec3  Bitangent;
in      vec2  UV;

uniform mat4  Model;
uniform mat3  ModelInverseTranspose;

out     vec4  worldPosition;
out     vec2  texCoords;
out     mat3  TBN;
out     float fogFac;

void SIN_applyTransforms()
{
    worldPosition   = Model                    * vec4(Position, 1.0);
    gl_Position     = AmbientIn.ViewProjection * worldPosition;

    float distance  = length(gl_Position.xyz);
    fogFac          = clamp( exp(-pow((distance*0.035), 2.5)), 0.0, 1.0 );

    texCoords       = UV;

    vec3 T          = normalize(ModelInverseTranspose * Tangent);
    vec3 B          = normalize(ModelInverseTranspose * Bitangent);
    vec3 N          = normalize(ModelInverseTranspose * Normal);

    TBN             = mat3(T, B, N);
}

)glsl"
;

const GLchar* SIN_ShaderBlock_FragTransform = 

R"glsl(

in vec4  worldPosition;
in vec2  texCoords;
in mat3  TBN;
in float fogFac;

)glsl"
;

#include "SIN_DefaultShader.h"

const GLchar* SIN_DefaultShader_source_v[1] = 
{
R"glsl(#version 150

in vec3      Position;
in vec3      Normal;
in vec3      Tangent;
in vec3      Bitangent;
in vec2      UV;

out vec2     texCoords;
out vec3     CamPos0;
out vec4     fragPos;
out mat3     TBN;

uniform mat4 Model;
uniform mat4 ViewProjection;
uniform mat3 ModelInverseTranspose;

uniform vec3 CamPos;

void main()
{

    vec4 worldPosition = Model * vec4(Position, 1.0);
    gl_Position        = ViewProjection * worldPosition;

    texCoords          = UV;

    vec3 T             = normalize(ModelInverseTranspose * Tangent);
    vec3 B             = normalize(ModelInverseTranspose * Bitangent);
    vec3 N             = normalize(ModelInverseTranspose * Normal);

    TBN                = mat3(T, B, N);

    fragPos            = vec4(worldPosition.xyz, 1);
    CamPos0            = CamPos;

}
)glsl"
};

const GLchar* SIN_DefaultShader_source_p[1] =
{
R"glsl(#version 150

in vec2   texCoords;
in vec4   fragPos;
in float  fogFac;
in vec3   CamPos0;
in mat3   TBN;

uniform sampler2D DiffuseMap;
uniform sampler2D ShadingInfo;
uniform sampler2D NormalMap;

uniform vec4   Ambient;
//uniform vec3 SunFwd;
uniform vec3   CamFwd;


void main()
{

    vec3 ambient     = vec3(Ambient.x, Ambient.y, Ambient.z) * Ambient.w;

    vec3 lightDir    = normalize(CamPos0 - fragPos.xyz);

    vec3 diffuse     = texture2D(DiffuseMap,  texCoords).rgb;
    vec3 shadinginfo = texture2D(ShadingInfo, texCoords).rgb;
    vec3 normal      = texture2D(NormalMap,   texCoords).rgb;
    normal           = normal * 2.0 - 1.0;
    normal           = normalize(TBN * normal);

    float ao         = shadinginfo.r;
    float softness   = shadinginfo.g;
    float metallic   = shadinginfo.b;

    float specfac = clamp(dot(reflect(-lightDir, normal), CamFwd), 0.0, 1.49);
    vec3 specular = (diffuse * specfac);

    float diff       = clamp(dot(lightDir, normal), 0.06, 1.49);
    diffuse         *= diff;

    vec3 refmapping  = normalize(reflect(CamFwd, normal));
    float metalshine = texture2D(DiffuseMap, refmapping.xy).r;

    vec3 composite   = ambient + (diffuse * ao) + (specular * softness)
                     + (vec3(metalshine, metalshine, metalshine) * metallic);

    gl_FragColor = vec4(composite, 1);

}
)glsl"
};

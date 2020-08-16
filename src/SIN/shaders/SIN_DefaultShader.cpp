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

uniform sampler2D shineRough;
uniform sampler2D shineSoft;

uniform vec4   Ambient;
//uniform vec3 SunFwd;
uniform vec3   CamFwd;

vec2 voodoo_UV(vec3 u, vec3 n)
{
    vec3  r = reflect(u, n);
    float m = 2 * sqrt( (r.x*r.x) + (r.y*r.y) + ((r.z+1.0)*(r.z+1.0)) );

    return vec2(r.x/m + 0.5, r.y/m + 0.5);
}

void main()
{

    vec3 ambient     = vec3(Ambient.x, Ambient.y, Ambient.z) * Ambient.w;

    vec3 lightDir    = normalize(CamPos0 - fragPos.xyz);

    vec3 diffuse     = texture2D(DiffuseMap,  texCoords).rgb;
    vec3 shadinginfo = texture2D(ShadingInfo, texCoords).rgb;
    vec3 normalmap   = texture2D(NormalMap,   texCoords).rgb;
    normalmap        = normalmap * 2.0 - 1.0;

    vec3 normal      = normalize(TBN * normalmap);
    vec3 softnormal  = normalize(TBN[2] + (normalmap * 0.35));

    float ao         = shadinginfo.r;
    float softness   = 1 - shadinginfo.g;
    float metallic   = shadinginfo.b;

    float eyeshd     = clamp(dot(-CamFwd, softnormal), 0.18, 1.49);
    eyeshd           = smoothstep(0.00, 0.23, eyeshd);

    float specfac    = clamp(dot(reflect(-lightDir, normal), CamFwd), 0.0, 1.49);
    vec3 specular    = (diffuse * specfac * softness);

    float diff       = clamp(dot(lightDir, normal), 0.06, 1.49);
    diffuse         += (softness * 0.25);
    diffuse         *= diff * ao * eyeshd;

    vec2 refUV       = voodoo_UV(CamFwd, softnormal);

    vec3 rougshine   = texture2D(shineRough, refUV).rgb * 0.75;
    vec3 softshine   = texture2D(shineSoft, refUV).rgb  * 0.75;

    vec3 refshine    = mix(rougshine, softshine, softness) * metallic;

    vec3 composite   = ambient + diffuse + specular + refshine;

    gl_FragColor = vec4(composite, 1);

}
)glsl"
};

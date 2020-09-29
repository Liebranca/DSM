#include "SIN_DefaultShader.h"

const GLchar* SIN_DefaultShader_source_v[1] = 
{
R"glsl(#version 150

const   uint  MAX_LIGHTS = 1;

in      vec3  Position;
in      vec3  Normal;
in      vec3  Tangent;
in      vec3  Bitangent;
in      vec2  UV;

out     vec2  texCoords;
out     vec3  CamPos0;
out     vec4  worldPosition;
out     vec4  lightSpacePosition;
out     vec3  lightDir;
out     vec3  viewPos;
out     mat3  TBN;
out     float fogFac;

uniform mat4  Model;
uniform mat4  ViewProjection;
uniform mat3  ModelInverseTranspose;

uniform uint  NUM_LIGHTS;

struct SIN_LIGHT {
    mat4 mat;
    vec3 position;
    vec3 dirn;
    vec4 color;
};

uniform SIN_LIGHT Lights[MAX_LIGHTS];

uniform vec3  CamPos;

void main()
{

    worldPosition      = Model * vec4(Position, 1.0);
    lightSpacePosition = Lights[0].mat * worldPosition;
    lightDir           = Lights[0].dirn;

    gl_Position        = ViewProjection * worldPosition;
    viewPos            = gl_Position.xyz;

    texCoords          = UV;

    vec3 T             = normalize(ModelInverseTranspose * Tangent);
    vec3 B             = normalize(ModelInverseTranspose * Bitangent);
    vec3 N             = normalize(ModelInverseTranspose * Normal);

    TBN                = mat3(T, B, N);

    CamPos0            = CamPos;

    float distance     = length(gl_Position.xyz);
    fogFac             = clamp( exp(-pow((distance*0.035), 2.5)), 0.0, 1.0 );

}
)glsl"
};

const GLchar* SIN_DefaultShader_source_p[1] =
{
R"glsl(#version 150

in      vec2      texCoords;
in      vec4      worldPosition;
in      vec4      lightSpacePosition;
in      vec3      lightDir;
in      vec3      viewPos;
in      float     fogFac;
in      vec3      CamPos0;
in      mat3      TBN;

uniform sampler2D shineRough;
uniform sampler2D shineSoft;
uniform sampler2D DepthMap;

uniform sampler2D DiffuseMap;
uniform sampler2D ShadingInfo;
uniform sampler2D NormalMap;

uniform vec4      Ambient;
uniform vec3      CamFwd;

float zNear      = 0.01;
float zFar       = 10.0;
float shadowBias = 0.05;

vec2 voodoo_UV(vec3 u, vec3 n)
{
    vec3  r = reflect(u, n);
    float m = 2 * sqrt( (r.x*r.x) + (r.y*r.y) + ((r.z+1.0)*(r.z+1.0)) );

    return vec2(r.x/m + 0.5, r.y/m + 0.5);
}

vec3 vec_overlay(vec3 a, vec3 b)
{

    vec3 blend = vec3(0,0,0);

    blend.r    = a.r < 0.5 ? (2.0 * a.r * b.r) : (1.0 - 2.0 * (1.0 - a.r) * (1.0 - b.r));
    blend.g    = a.g < 0.5 ? (2.0 * a.g * b.g) : (1.0 - 2.0 * (1.0 - a.g) * (1.0 - b.g));
    blend.b    = a.b < 0.5 ? (2.0 * a.b * b.b) : (1.0 - 2.0 * (1.0 - a.b) * (1.0 - b.b));

    return blend;
}

float calcShadow()
{
    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    projCoords      = (projCoords * 0.5) + 0.5;

    if(projCoords.z > 1.0) { return 0; }

    float closestDepth = texture(DepthMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float shadow = currentDepth - shadowBias > closestDepth ? 1.0 : 0.0;

    return shadow;
}  

void main()
{
    vec3 skyColor    = vec3(Ambient.x, Ambient.y, Ambient.z);
    vec3 ambient     = skyColor * Ambient.w;

    // vec3 toCam    = normalize(CamPos0 - worldPosition.xyz);

    vec3 diffuse     = texture2D(DiffuseMap,  texCoords).rgb;
    vec3 shadinginfo = texture2D(ShadingInfo, texCoords).rgb;
    vec3 normalmap   = texture2D(NormalMap,   texCoords).rgb;

    normalmap        = (normalmap * 2.0) - 1.0;    
    vec3 normal      = normalize(TBN * normalmap);

    normalmap.rg    *= 0.35;
    vec3 softnormal  = normalize(TBN * normalize(normalmap));

    float ao         = shadinginfo.r;
    float cavity     = shadinginfo.g;
    float softness   = 1 - cavity;
    float metallic   = shadinginfo.b;

    float specfac    = clamp(dot(reflect(-lightDir, normal), CamFwd), 0.0, 1.0);
    vec3 specular    = (diffuse * specfac) * (0.65 * softness);

    vec3 refshine    = vec3(0,0,0);

    // if(ao < 0.8) { ao *= 0.90; }

    if(metallic)
    {
        vec2 refUV       = voodoo_UV(CamFwd, softnormal);

        vec3 rougshine   = texture2D(shineRough, refUV).rgb * 0.75;
        vec3 softshine   = texture2D(shineSoft, refUV).rgb  * 0.75;

        refshine         = mix(rougshine, softshine, softness) * metallic;
        refshine         = vec_overlay(refshine, diffuse);
    }

    float diff       = smoothstep(0.06, 0.8, dot(-lightDir, softnormal));
    diff             = clamp(diff, 0.75, 1.0);
    diffuse          = clamp(diffuse * diff * ao, 0.06, 1.0) + (cavity*0.5);

    float bounce     = 1 - dot(normal, lightDir);

    shadowBias       = max(0.001 * bounce, 0.0005);

    float shadow     = clamp(bounce, 0.2, 1) * clamp((1 - calcShadow()) + (bounce * 0.2), 0.2, 1);
    vec3 composite   = clamp(ambient + shadow * (diffuse + specular) + refshine, 0.06, 1.49);
    composite        = mix(skyColor, composite, fogFac);

    gl_FragColor     = vec4(composite, 1);

}
)glsl"
};

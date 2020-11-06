#include "SIN_DefaultShader.h"
#include "SIN_ShaderBlocks_DefHed.h"
#include "SIN_ShaderBlocks_Ambient.h"
#include "SIN_ShaderBlocks_Transform.h"
#include "SIN_ShaderBlocks_Light.h"

const char* SIN_DefaultShader_source_v[6] = 
{

SIN_ShaderBlock_DefHed,
SIN_ShaderBlock_VertAmbient,
SIN_ShaderBlock_VertTransform,
SIN_ShaderBlock_VertLightIn,
SIN_ShaderBlock_VertLightOut,

R"glsl(

void main()
{
    SIN_applyTransforms    ();
    SIN_setAmbient         ();
    SIN_calcFragLight      ();
}

)glsl"
};

const char* SIN_DefaultShader_source_f[5] =
{

SIN_ShaderBlock_DefHed,
SIN_ShaderBlock_FragAmbient,
SIN_ShaderBlock_FragTransform,
SIN_ShaderBlock_FragLight,

R"glsl(

uniform sampler2DArray ENV;
uniform sampler2DArray Surface;

vec2 spheremap_UV(vec3 u, vec3 n)
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

void main()
{
    vec3 skyColor    = vec3(AmbientColor.x, AmbientColor.y, AmbientColor.z);
    vec3 ambient     = skyColor * AmbientColor.w;

    vec3 diffuse     = texture(Surface, vec3(texCoords, 0)).rgb;
    vec3 shadinginfo = texture(Surface, vec3(texCoords, 1)).rgb;
    vec3 normalmap   = texture(Surface, vec3(texCoords, 2)).rgb;

    normalmap        = (normalmap * 2.0) - 1.0;
    vec3 normal      = normalize(TBN * normalmap);

    normalmap.rg    *= 0.35;
    vec3 softnormal  = normalize(TBN * normalize(normalmap));

    float ao         = shadinginfo.r;
    float cavity     = shadinginfo.g;
    float softness   = 1 - cavity;
    float metallic   = shadinginfo.b;

    vec4 lighting    = SIN_calcLight(normal, softnormal);

    float shadow     = lighting.x;
    float light      = lighting.y;
    float diff       = lighting.z;
    float spec       = lighting.w;

    vec3 specular    = (diffuse * spec) * (0.65 * softness);
    vec3 refshine    = vec3(0,0,0);

    /*if(metallic)
    {
        vec2 refUV     = spheremap_UV(CamFwd, softnormal);

        vec3 rougshine = texture(ENV, vec3(refUV, 0)).rgb * 0.75;
        vec3 softshine = texture(ENV, vec3(refUV, 1)).rgb * 0.75;

        refshine       = mix(rougshine, softshine, softness) * metallic;
        refshine       = vec_overlay(refshine, diffuse);
    }*/

    diffuse          = clamp(diffuse * diff * ao, 0.06, 1.0) + (cavity*0.5);

    vec3 composite   = clamp(ambient + (shadow * (diffuse + specular)) + light + refshine, 0.06, 1.49);
    composite        = mix  (skyColor, composite, fogFac);

    gl_FragColor     = vec4 (composite, 1);

}
)glsl"
};

const shaderParams SIN_DefaultShader =
{

    SIN_DefaultShader_source_v,                     // Vertex sources
    SIN_DefaultShader_source_f,                     // Fragment sources

    { SIN_ShaderBlock_TransformAttribs [0],        // Attributes
      SIN_ShaderBlock_TransformAttribs [1],
      SIN_ShaderBlock_TransformAttribs [2],
      SIN_ShaderBlock_TransformAttribs [3],
      SIN_ShaderBlock_TransformAttribs [4]  },

    { 
      SIN_ShaderBlock_TransformUniforms[0],
      SIN_ShaderBlock_TransformUniforms[1]  },      // Uniforms

    { SIN_ShaderBlock_AmbientUniforms  [0],
      SIN_ShaderBlock_LightUniforms    [0]  },      // UBOs

    { SIN_ShaderBlock_LightSamplers    [0],
      "ENV", "Surface"                      },      // Samplers

    6,                                              // Number of vertex shader blocks
    5,                                              // Number of fragment shader blocks
    5,                                              // Number of attributes
    2,                                              // Number of uniforms
    2,                                              // Number of UBOs
    3,                                              // Number of samplers

    SIN_SBFTRANSFORMA | SIN_SBFTRANSFORMB |         // Flags
    SIN_SBFAMBIENT    | SIN_SBFLIGHTING

};

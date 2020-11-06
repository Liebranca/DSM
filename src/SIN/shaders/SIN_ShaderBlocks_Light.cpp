#include "SIN_ShaderBlocks_Light.h"

const GLchar* SIN_ShaderBlock_LightUniforms[1] =
{
    "Lights"
};

const GLchar* SIN_ShaderBlock_LightSamplers[2] =
{
    "ShadowMap",
};

const GLchar* SIN_ShaderBlock_VertLightIn = 

R"glsl(

const uint SIN_MAX_LAMPS  =  4;

struct SIN_LAMP {

    mat4  mat;

    vec4  position;     // posx, posy, posz, rea
    vec4  color;        // colr, colg, colb, intensity
    vec4  dir;          // dirx, diry, dirz, 1
};

layout (std140) uniform Lights {
    
    SIN_LAMP lamps[SIN_MAX_LAMPS];
    uint     num_lamps;
} LightIn;

)glsl"
;

const GLchar* SIN_ShaderBlock_VertLightOut = 

R"glsl(

struct SIN_LIGHTOUT {
    vec4  position;                     // posx, posy, posz, posw
    vec4  color;                        // colr, colg, colb, atten
    vec3  dir;                          // dirx, diry, dirz
};

out LIGHTDATA { SIN_LIGHTOUT lamps[SIN_MAX_LAMPS]; } FragLight;

flat out uint num_lamps;

void SIN_calcFragLight()
{

    num_lamps = LightIn.num_lamps;    

    for(uint i = 0; i < num_lamps; i++)
    {
        float rea                   = LightIn.lamps[i].position.w;
        float intensity             = LightIn.lamps[i].color.a;
        float dist                  = distance(LightIn.lamps[i].position.xyz, worldPosition.xyz);

        float atten                 = clamp(intensity - (dist * dist) / (rea * rea), 0.0, 1.0);

        FragLight.lamps[i].position = 0.5 + (0.5 * (LightIn.lamps[i].mat * worldPosition));
        FragLight.lamps[i].dir      = LightIn.lamps[i].dir.xyz;
        FragLight.lamps[i].color    = vec4(LightIn.lamps[i].color.rgb * intensity, atten);
    }
}

)glsl"
;

const GLchar* SIN_ShaderBlock_FragLight = 

R"glsl(

const uint SIN_MAX_LAMPS  = 4;

struct SIN_LIGHTIN {
    vec4  position;                     // posx, posy, posz, posw
    vec4  color;                        // colr, colg, colb, atten
    vec3  dir;                          // dirx, diry, dirz
};

in LIGHTDATA { SIN_LIGHTIN lamps[SIN_MAX_LAMPS]; } FragLight;

flat in uint num_lamps;

uniform sampler2DArrayShadow ShadowMap;

float   shadowBias = 0.001;

float SIN_calcShadow(vec4 pos, float slice)
{
    vec4 projCoords  = vec4(pos.xyz / pos.w, 1);
    projCoords.xyw   = projCoords.xyz;
    projCoords.z     = slice;

    float depth      = texture(ShadowMap, projCoords).x;
    if(projCoords.w + shadowBias > depth) { return 1; }
    
    return 0;
}

vec4 SIN_calcLight(vec3 normal, vec3 softnormal)
{

    if(num_lamps == 0) { return vec4(1.0, 0.0, 1.0, 0.0); }

    float weight     = 1.0/num_lamps;

    float shadow     = 1.0;
    float light      = 0.0;
    float diff       = 1.0;
    float spec       = 0.0;

    for(uint i = 0; i < num_lamps; i++)
    {
        vec4 pos     = FragLight.lamps[i].position;
        vec3 dir     = FragLight.lamps[i].dir;
        vec3 color   = FragLight.lamps[i].color.rgb;

        float atten  = FragLight.lamps[i].color.a;

        float bounce = 1 - dot(-normal, dir);
        shadowBias   = max(0.001 * bounce, 0.0005);
        float indark = SIN_calcShadow(pos, float(i));

        if(indark) { shadow -= ((0.8 * atten) * weight) + ((bounce * 0.5) * weight); }
        else       { light  += ( 0.2 * atten) * weight;                              }

        diff        -= (smoothstep(0.06, 0.8, dot(-dir, softnormal)) * atten) * weight;
        spec        += (dot(reflect(-dir, normal), CamFwd)           * atten) * weight;
    }

    shadow = clamp(shadow, 0.20, 0.85);
    light  = clamp(light,  0.00, 2.00);
    diff   = clamp(diff,   0.75, 1.00);
    spec   = clamp(spec,   0.00, 1.00);

    return vec4(shadow, light, diff, spec);
}

)glsl"
;

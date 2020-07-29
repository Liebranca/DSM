#include "SIN_DefaultShader.h"

const GLchar* SIN_DefaultShader_source_v[] = 
{
R"glsl(#version 150

attribute vec4  Position;
attribute vec3  Normal;
attribute vec2  UV;

varying   vec2  texCoords;
varying   vec3  fragNormal;
varying   float fogFac;

uniform   mat4  Model;
uniform   mat4  ViewProjection;
uniform   mat3  ModelInverseTranspose;

void main()
{

    vec4 worldPosition = Model * Position;
    gl_Position        = ViewProjection * worldPosition;
    fragNormal         = normalize(ModelInverseTranspose * Normal);
    texCoords          = UV;

    float distance     = length(gl_Position.xyz);
    fogFac             = clamp( exp(-pow((distance*0.035), 2.5)), 0.0, 1.0 );

}
)glsl"
};

const GLchar* SIN_DefaultShader_source_p[] =
{
R"glsl(#version 150

varying vec2      texCoords;
varying vec3      fragNormal;
varying float     fogFac;

uniform sampler2D tex;
uniform vec3      SunEye;
uniform vec4      AmbientColor;

void main()
{
    float base   = clamp(dot(-(SunEye)+vec3(0,0.5,0), fragNormal), 0.0, 1.49);

    float black  = smoothstep(0.01, 0.8, base);

    vec4 diffuse = texture2D(tex, texCoords);
    //vec4 lighting = diffuse * lightColor + vec4(0.01f, 0.01f, 0.02f, 1) + (AmbientColor * 0.1f);
    //vec4 composite = mix(AmbientColor*4, diffuse * lighting, fogFac);

    //composite *= 1 + ((1 - length(composite)) * 0.1f);

    //gl_FragColor = composite;
    gl_FragColor = diffuse;

}
)glsl"
};

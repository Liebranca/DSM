#include "SIN_DefaultShader.h"

const char* SIN_DefaultShader_v =

"#version 130\n\
\
in vec4   Position;\n\
in vec3   Normal;\n\
in vec2   UV;\n\
\
out       vec2  texCoords;\n\
out       vec3  fragNormal;\n\
out       float fogFac;\n\
\
uniform   mat4  Model;\n\
uniform   mat4  ViewProjection;\n\
uniform   mat3  ModelInverseTranspose;\n\
\
struct vertex_data {\
\
    vec4 position;\
    vec3 normal;\
    vec2 uv;\
\
    int  matid;\
\
};\n\
\
void main()\
{\
\
    vec4 worldPosition = Model * Position;\
    gl_Position        = ViewProjection * worldPosition;\
    fragNormal         = normalize(ModelInverseTranspose * Normal);\
    texCoords          = UV;\
\
    float distance     = length(gl_Position.xyz);\
    fogFac             = clamp( exp(-pow((distance*0.035), 2.5)), 0.0, 1.0 );\
\
    //lightColor         = calcLight(vec4(0,0,0,1), worldPosition, fragNormal);\n\
\
}\n";

const char* SIN_DefaultShader_f =

"#version 130\n\
\
in      vec2      texCoords;\n\
in      vec3      fragNormal;\n\
in      float     fogFac;\n\
\
uniform sampler2D tex;\n\
uniform vec3      SunEye;\n\
uniform vec4      AmbientColor;\n\
\n\
void main()\
{\
    float base = clamp(dot(-(SunEye)+vec3(0,0.5,0), fragNormal), 0.0, 1.49);\
\
    float black = smoothstep(0.01, 0.8, base);\
\
    vec4 diffuse = texture2D(tex, texCoords);\
    //vec4 lighting = diffuse * lightColor + vec4(0.01f, 0.01f, 0.02f, 1) + (AmbientColor * 0.1f);\n\
    //vec4 composite = mix(AmbientColor*4, diffuse * lighting, fogFac);\n\
\
    //composite *= 1 + ((1 - length(composite)) * 0.1f);\n\
\
    //gl_FragColor = composite;\n\
    gl_FragColor = diffuse;\
\
}\n";

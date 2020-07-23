#include "SIN_DefaultShader.h"

const char* SIN_DefaultShader_v =

R"glsl(#version 130

attribute uint  FRAC[2];

out       vec2  texCoords;
out       vec3  fragNormal;
out       float fogFac;

uniform   mat4  Model;
uniform   mat4  ViewProjection;
uniform   mat3  ModelInverseTranspose;

struct vertex_data {

    vec4 position;
    vec3 normal;
    vec2 uv;

    uint matid;

}

vertex_data frac_decode(uint b1, uint b2)       {

    vertex_data vertex;

    uint px         =       (b1 & 255);
    uint py         =       (b1 & 65280)      >> 8;
    uint pz         =       (b1 & 16711680)   >> 16;

    uint uv         =       (b1 & 4278190080) >> 24;

    vertex.position = vec4( (px & 127) * (1 - (2 * (px & 1))) * 0.03125f,
                            (py & 127) * (1 - (2 * (py & 1))) * 0.03125f,
                            (pz & 127) * (1 - (2 * (pz & 1))) * 0.03125f,
                            1.0f                                             );

    vertex.uv       = vec2( (uv & 15 ) * 0.0625f,
                            (uv & 240) * 0.0625f )

    uint nx         =       (b2 & 255);
    uint ny         =       (b2 & 65280)      >> 8;
    uint nz         =       (b2 & 16711680)   >> 16;

    vertex.matid    =       (b2 & 4278190080) >> 24;

    vertex.normal   = vec3( (nx & 127) * (1 - (2 * (nx & 1))) * 0.03125f,
                            (ny & 127) * (1 - (2 * (ny & 1))) * 0.03125f,
                            (nz & 127) * (1 - (2 * (nz & 1))) * 0.03125f     );

    return vertex;                                                                                                      }

void main()
{

    vertex_data = frac_decode(FRAC[0], FRAC[1])

    vec4 worldPosition = Model * vertex_data.position;
    gl_Position        = ViewProjection * worldPosition;
    fragNormal         = normalize(ModelInverseTranspose * vertex_data.normal);
    texCoords          = vertex_data.uv;
    
    float distance     = length(gl_Position.xyz);
    fogFac             = clamp( exp(-pow((distance*0.035), 2.5)), 0.0, 1.0 );

    //lightColor         = calcLight(vec4(0,0,0,1), worldPosition, fragNormal);

})glsl";

const char* SIN_DefaultShader_f =

R"glsl(#version 130

in      vec2      texCoords;
in      vec3      fragNormal;
in      float     fogFac;

uniform sampler2D tex;
uniform vec3      SunEye;
uniform vec4      AmbientColor;

void main()
{
    float base = clamp(dot(-(SunEye)+vec3(0,0.5,0), fragNormal), 0.0, 1.49);

    float black = smoothstep(0.01, 0.8, base);

    vec4 diffuse = texture2D(tex, texCoords);
    //vec4 lighting = diffuse * lightColor + vec4(0.01f, 0.01f, 0.02f, 1) + (AmbientColor * 0.1f);
    //vec4 composite = mix(AmbientColor*4, diffuse * lighting, fogFac);

    //composite *= 1 + ((1 - length(composite)) * 0.1f);

    //gl_FragColor = composite;
    gl_FragColor = diffuse;

})glsl";


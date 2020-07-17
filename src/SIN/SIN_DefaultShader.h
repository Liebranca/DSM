#ifndef __SIN_DEFAULT_SHADER_H__
#define __SIN_DEFAULT_SHADER_H__

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

const char* SIN_DefaultShader_v =

     R"glsl(#version 460

    attribute vec3 POSITION;
    attribute vec3 NORMAL;
    attribute vec2 UV;

    out vec2  TEXCOORDS;
    out vec3  FRAGNORMAL;
    out vec4  LIGHTCOLOR;
    out float FOGFACTOR;

    struct Light {
        vec4 position;
        vec4 color;
        float radius;
    };

    uniform mat4 MODEL;
    uniform mat3 INVERSE_TRANSPOSE;
    uniform mat4 VIEWPROJECTION;

    uniform uint NUM_LIGHTS;
    uniform Light LIGHTS[12];

    vec4 calcLight(vec4 col, vec4 pos, vec3 vertNormal) {

        for(uint i = 0; i < NUM_LIGHTS; i++)
        {

            vec4 lpos = LIGHTS[i].position;
            vec4 toCam = lpos - pos;

            float dist = length((toCam).xyz);
            dist = clamp( exp(-pow(((dist * 0.1) / LIGHTS[i].radius), 5)), 0.0, 1.0 );

            if(vertNormal.y == 1)               { toCam.y *= 5;                                                         }

            vec4 contrib = LIGHTS[i].color * dot(normalize(toCam.xyz), vertNormal) * (4 * cos(dist)) * dist;

            col += contrib;
        }

        col.r = clamp(col.r, 0.1f, 1.49f);
        col.g = clamp(col.g, 0.1f, 1.49f);
        col.b = clamp(col.b, 0.2f, 1.49f);

        return col;                                                                                                     }

    )glsl";

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __SIN_DEFAULT_SHADER_H__


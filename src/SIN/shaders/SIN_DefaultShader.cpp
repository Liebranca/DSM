#include "SIN_DefaultShader.h"

const GLchar* SIN_DefaultShader_source_v[1] = 
{
R"glsl(#version 150

in vec3      Position;
in vec3      Normal;
in vec2      UV;

out vec2     texCoords;
out vec3     fragNormal;
out vec3     CamPos0;
out vec4     fragPos;
out float    fogFac;

uniform mat4 Model;
uniform mat4 ViewProjection;
uniform mat3 ModelInverseTranspose;

uniform vec3 CamPos;

void main()
{

    vec4 worldPosition = Model * vec4(Position, 1.0);
    gl_Position        = ViewProjection * worldPosition;

    fragNormal         = normalize(ModelInverseTranspose * Normal);
    texCoords          = UV;

    vec4 toCam         = vec4(CamPos - worldPosition.xyz, 1);
    float dist         = length(toCam.xyz);
    dist               = clamp( exp(-pow(((dist * 0.1) / 10), 5)), 0.0, 1.0 );
    float contrib      = dot(normalize(toCam.xyz), fragNormal) * (4 * cos(dist)) * dist;

    fragPos            = vec4(worldPosition.xyz, contrib);

    float distance     = length(gl_Position.xyz);
    fogFac             = clamp( exp(-pow((distance*0.035), 2.5)), 0.0, 1.0 );

    CamPos0            = CamPos;

}
)glsl"
};

const GLchar* SIN_DefaultShader_source_p[1] =
{
R"glsl(#version 150

in vec2   texCoords;
in vec3   fragNormal;
in vec4   fragPos;
in float  fogFac;

in vec3   CamPos0;

uniform sampler2D DiffuseMap;

uniform vec4   Ambient;
//uniform vec3 SunFwd;
uniform vec3   CamFwd;


void main()
{

    vec3 ambient   = vec3(Ambient.x, Ambient.y, Ambient.z) * Ambient.w;
    vec3 lightDir = normalize(CamPos0 - fragPos.xyz);

    float shd      = clamp(dot(lightDir - CamFwd, fragNormal), 0.06, 1.49);
    shd            = smoothstep(0.06, 1.49, shd);

    // ambient       /= cos(shd*1.25);

    float contra   = fragPos.w;

    //float base   = clamp(dot(-(SunEye)+vec3(0,0.5,0), fragNormal), 0.0, 1.49);

    //float black  = smoothstep(0.01, 0.8, base);

    vec4 diffuse = texture2D(DiffuseMap, texCoords);

    //vec4 lighting = diffuse * lightColor + vec4(0.01f, 0.01f, 0.02f, 1) + (AmbientColor * 0.1f);
    //vec4 composite = mix(AmbientColor*4, diffuse * lighting, fogFac);

    //composite *= 1 + ((1 - length(composite)) * 0.1f);

    //gl_FragColor = composite;

    float diff     = clamp(dot(lightDir, fragNormal), 0.06, 1.49);
    // diff           = smoothstep(0.06, 0.49, diff);
    diffuse       *= diff;

    vec3 composite = vec3(diffuse.xyz + ambient);
    
    gl_FragColor   =  vec4(composite, 1);

}
)glsl"
};

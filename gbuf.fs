#version 330 core

in vec3 vPos;
in vec3 vNormal;
in vec2 vTex;
in vec4 glPos;

layout (location = 0) out vec4 gPosDepth;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gKa;
layout (location = 3) out vec4 gKd;
layout (location = 4) out vec4 gKs;
layout (location = 5) out vec4 gNs;

uniform vec3 color_ambient;
uniform int usetex_ambient;
uniform sampler2D texture_ambient1;
uniform vec3 color_diffuse;
uniform int usetex_diffuse;
uniform sampler2D texture_diffuse1;
uniform vec3 color_specular;
uniform int usetex_specular;
uniform sampler2D texture_specular1;
uniform float shininess;


struct PointLight 
{
    vec3 pos;
    vec3 val; // intensity
};

uniform int n_point_light;
uniform PointLight point_light[4];

void main()
{
    vec3 color;
    float Ns = shininess;
    vec3 Ka = texture(texture_ambient1, vTex).xyz;
    if(usetex_ambient==0) Ka=color_ambient;
    Ka = pow(Ka, vec3(2.2));
    vec3 Kd = texture(texture_diffuse1, vTex).xyz;
    if(usetex_diffuse==0) Kd=color_diffuse;
    Kd = pow(Kd, vec3(2.2));
    vec3 Ks = texture(texture_specular1, vTex).xyz;
    if(usetex_specular==0) Ks=color_specular;
    Ks = pow(Ks, vec3(2.2));
    gPosDepth = vec4(vPos, glPos.z / glPos.w);
    gNormal = vec4(vNormal, 1.0);
    gKa = vec4(Ka, 1.0);
    gKd = vec4(Kd, 1.0);
    gKs = vec4(Ks, 1.0);
    gNs = vec4(Ns,Ns,Ns,Ns);
}
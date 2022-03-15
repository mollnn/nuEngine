#version 330 core

in vec3 vPos;
in vec3 vNormal;
in vec2 vTex;

layout (location = 0) out vec3 gPos;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gKa;
layout (location = 3) out vec3 gKd;
layout (location = 4) out vec3 gKs;
layout (location = 5) out vec3 gNs;

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
    vec3 Kd = texture(texture_diffuse1, vTex).xyz;
    if(usetex_diffuse==0) Kd=color_diffuse;
    vec3 Ks = texture(texture_specular1, vTex).xyz;
    if(usetex_specular==0) Ks=color_specular;
    gPos = vPos;
    gNormal = vNormal;
    gKa = Ka;
    gKd = Kd;
    gKs = Ks;
    gNs = vec3(Ns,Ns,Ns);
}
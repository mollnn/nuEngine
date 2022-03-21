#version 330 core

in vec3 v_pos;
in vec3 v_normal;
in vec2 v_texcoord;
in vec3 v_tangent;
in vec3 v_bitangent;
in vec4 v_pos4;

layout (location = 0) out vec4 gbuf_posdepth;
layout (location = 1) out vec4 gbuf_normal;
layout (location = 2) out vec4 gbuf_Ka;
layout (location = 3) out vec4 gbuf_Kd;
layout (location = 4) out vec4 gbuf_Ks;
layout (location = 5) out vec4 gbuf_Ns;
layout (location = 6) out vec4 gbuf_tangent;
layout (location = 7) out vec4 gbuf_bitangent;

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
    vec3 Ka = texture(texture_ambient1, v_texcoord).xyz;
    if(usetex_ambient==0) Ka=color_ambient;
    Ka = pow(Ka, vec3(2.2));
    vec3 Kd = texture(texture_diffuse1, v_texcoord).xyz;
    if(usetex_diffuse==0) Kd=color_diffuse;
    Kd = pow(Kd, vec3(2.2));
    vec3 Ks = texture(texture_specular1, v_texcoord).xyz;
    if(usetex_specular==0) Ks=color_specular;
    Ks = pow(Ks, vec3(2.2));
    gbuf_posdepth = vec4(v_pos, v_pos4.z / v_pos4.w);
    gbuf_normal = vec4(normalize(v_normal), 1.0);
    gbuf_Ka = vec4(Ka, 1.0);
    gbuf_Kd = vec4(Kd, 1.0);
    gbuf_Ks = vec4(Ks, 1.0);
    gbuf_Ns = vec4(Ns,Ns,Ns,Ns);
    gbuf_tangent = vec4(v_tangent, 1.0);
    gbuf_bitangent = vec4(v_bitangent, 1.0);
}
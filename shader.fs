#version 330 core

in vec3 vPos;
in vec3 vNormal;
in vec2 vTex;

out vec4 FragColor;

uniform sampler2D diffuse_texture1;

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
    for(int i=0;i<n_point_light;i++)
    {
        vec3 Pl = point_light[i].pos;
        vec3 Ps = vPos;
        vec3 n = vNormal;
        vec3 Ei = point_light[i].val / dot(Pl-Ps, Pl-Ps);
        vec3 Wi = normalize(Pl-Ps);
        vec3 Kd = texture(diffuse_texture1, vTex).xyz;
        vec3 Ld = 1.0 / 3.14159 * Kd * Ei * max(0.0, dot(Wi, n));
        color += Ld;
    }
    FragColor = vec4(color, 1.0);
}
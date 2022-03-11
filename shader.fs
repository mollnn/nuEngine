#version 330 core

in vec3 vPos;
in vec3 vNormal;
in vec2 vTex;

out vec4 FragColor;

uniform sampler2D tex;

struct PointLight 
{
    vec3 pos;
    vec3 val;
};

uniform int n_point_light;
uniform PointLight point_light[4];

void main()
{
    vec3 color;
    for(int i=0;i<n_point_light;i++)
    {
        vec3 Kd = texture(tex, vTex).xyz;
        vec3 Id = 1.0 / 3.14159 * Kd * point_light[i].val / dot(point_light[i].pos - vPos, point_light[i].pos - vPos);
        color += Id;
    }
    FragColor = vec4(color, 1.0);
}
#version 330 core

in vec3 vPos;
in vec3 vNormal;
in vec2 vTex;

out vec4 FragColor;

uniform sampler2D tex;
uniform vec3 light_pos;
uniform vec3 light_int;

void main()
{
    vec3 Kd = texture(tex, vTex).xyz;
    vec3 Id = 1.0 / 3.14159 * Kd * light_int / dot(light_pos - vPos, light_pos - vPos);
    FragColor = vec4(Id, 1.0);
}
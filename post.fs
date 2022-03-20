#version 330 core

in vec2 vTex;

out vec4 FragColor;

uniform sampler2D lighting;
uniform sampler2D ssr;
uniform sampler2D rsm;

layout (std140) uniform ub_common
{
    uniform float rnds[1024];
};

uniform vec3 camera_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float near;
uniform float far;


void main()
{
    vec3 radiance_lighting = texture(lighting, vTex).xyz;
    vec3 radiance_ssr = texture(ssr, vTex).xyz;
    vec3 radiance_rsm = texture(rsm, vTex).xyz;

    vec3 radiance = radiance_lighting + radiance_rsm + radiance_ssr;
    vec3 color = pow(radiance, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
#version 330 core

in vec2 v_texcoord;

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
    int screen_width = 1920;
    int screen_height = 1080;

    vec3 radiance_lighting = texture(lighting, v_texcoord).xyz;
    vec3 radiance_ssr = texture(ssr, v_texcoord).xyz;
    vec3 radiance_rsm = texture(rsm, v_texcoord).xyz * 0.1;
    for(int i=-1;i<=1;i++)
    {
        for(int j=-1;j<=1;j++)
        {
            radiance_rsm += texture(rsm, v_texcoord + vec2(i,j) / vec2(screen_width, screen_height)).xyz * 0.1;
        }
    }

    vec3 radiance = radiance_lighting + radiance_rsm + radiance_ssr;
    vec3 color = pow(radiance, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
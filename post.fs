#version 330 core

in vec2 v_texcoord;

out vec4 FragColor;

uniform sampler2D gbuf0;
uniform sampler2D gbuf1;
uniform sampler2D lighting;
uniform sampler2D ssr;
uniform sampler2D rsm;

layout(std140) uniform ub_common {
    uniform float rnds[1024];
};

uniform vec3 camera_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float near;
uniform float far;

void main() {
    int screen_width = 1920;
    int screen_height = 1080;

    vec3 radiance_lighting = texture(lighting, v_texcoord).xyz;
    vec3 radiance_ssr = texture(ssr, v_texcoord).xyz;
    vec3 radiance_rsm = vec3(0.0);
    vec3 g0 = texture(gbuf0, v_texcoord).xyz;
    vec3 g1 = texture(gbuf1, v_texcoord).xyz;

    // Joint Bilateral Filtering

    float sw = 0.0;
    for(int i = -3; i <= 3; i++) {
        for(int j = -3; j <= 3; j++) {
            vec3 tg0 = texture(gbuf0, v_texcoord + vec2(i, j) / vec2(screen_width, screen_height)).xyz;
            vec3 tg1 = texture(gbuf1, v_texcoord + vec2(i, j) / vec2(screen_width, screen_height)).xyz;
            float w = exp(-(i * i + j * j) * 0.5 - pow(length(tg0 - g0), 2) - pow(length(tg1 - g1), 2));    // approximation of gaussian
            radiance_rsm += texture(rsm, v_texcoord + vec2(i, j) / vec2(screen_width, screen_height)).xyz * w;
            sw += w;
        }
    }
    radiance_rsm /= sw;

    vec3 radiance = radiance_lighting + radiance_rsm + radiance_ssr;
    vec3 color = pow(radiance, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
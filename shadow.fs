#version 330 core

in vec3 v_pos;
in vec3 v_normal;
in vec2 v_texcoord;
layout (location = 0) out vec3 gbuf_pos;
layout (location = 1) out vec3 gbuf_normal;
layout (location = 2) out vec3 gbuf_flux;
uniform vec3 light_int;
uniform vec3 light_pos;
uniform float shadow_limit;

uniform vec3 color_diffuse;
uniform int usetex_diffuse;
uniform sampler2D texture_diffuse1;

void main()
{
    float light_dist = length(light_pos - v_pos) / shadow_limit;
    gl_FragDepth = light_dist;
    gbuf_pos = v_pos;
    gbuf_normal = normalize(v_normal);
    // if(length(v_normal)<0.1) gbuf_normal=vec3(1.0,0.0,0.0);

    vec3 n = gbuf_normal;

    vec3 Kd = texture(texture_diffuse1, v_texcoord).xyz;
    if(usetex_diffuse==0) Kd=color_diffuse;
    Kd = pow(Kd, vec3(2.2));

    vec3 Pl = light_pos;
    vec3 Ps = v_pos;
    vec3 Psl = Pl-Ps;
    float dsl = length(Psl);
    vec3 Ei = light_int / dsl / dsl;
    vec3 Wi = Psl / dsl;
    float cosWi = dot(Wi, n);
    vec3 Ld = 1.0 / 3.14159 * Kd * Ei * max(0.0, cosWi);
    float A = 1.0 * dsl * dsl / max(1e-4, cosWi);   // w0 is eliminated
    vec3 Phi = Ld * 3.14159 * A;

    gbuf_flux = Phi;
}
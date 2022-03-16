#version 330 core

in vec2 vTex;

out vec4 FragColor;

uniform sampler2D gbuf0;
uniform sampler2D gbuf1;
uniform sampler2D gbuf2;
uniform sampler2D gbuf3;
uniform sampler2D gbuf4;
uniform sampler2D gbuf5;
uniform sampler2D ao;

uniform vec3 rnd_kernel[64];

uniform samplerCube shadow_map;
uniform samplerCube shadow_map_pos;
uniform samplerCube shadow_map_normal;
uniform samplerCube shadow_map_flux;
uniform float shadowLimit;

uniform vec3 ambient;
uniform vec3 camera_pos;

struct PointLight 
{
    vec3 pos;
    vec3 val; // intensity
};

uniform int n_point_light;
uniform PointLight point_light[4];

void main()
{
    vec3 vPos = texture(gbuf0, vTex).xyz;
    vec3 vNormal = texture(gbuf1, vTex).xyz;
    vec3 Ka = texture(gbuf2, vTex).xyz;
    vec3 Kd = texture(gbuf3, vTex).xyz;
    vec3 Ks = texture(gbuf4, vTex).xyz;
    float Ns = texture(gbuf5, vTex).x;

    vec3 color = ambient * Ka * (texture(ao, vTex).r);
    vec3 Ps = vPos;
    vec3 n = vNormal;
    for(int i=0;i<n_point_light;i++)
    {
        vec3 Pl = point_light[i].pos;
        vec3 Ei = point_light[i].val / dot(Pl-Ps, Pl-Ps);
        vec3 Wi = normalize(Pl-Ps);
        vec3 Wo = normalize(camera_pos-Ps);
        vec3 h = normalize(Wi + Wo);
        vec3 Ld = 1.0 / 3.14159 * Kd * Ei * max(0.0, dot(Wi, n));
        vec3 Ls = (Ns + 2.0) / 8 / 3.14159 * Ks * Ei * pow(max(0.0, dot(h, n)), Ns);
        float vis = 1.0;
        if(i==0)
        {
            vec3 dp = vPos - point_light[0].pos;
            float d0 = texture(shadow_map, dp).r * shadowLimit;
            float d = length(dp);
            vis = d- d0 > 0.05 ? 0.0 : 1.0;
        }
        color += (Ld + Ls) * vis;
    }

    // color = vec3(0.0, 0.0, 0.0);

    // RSM 
    vec3 rsm_contribution;
    for(int i=0;i<64;i++)
    {
        vec3 dp = vPos - point_light[0].pos;
        vec3 dir = normalize(dp) + rnd_kernel[i] * 0.02;
        dir = normalize(dir);

        vec3 sample_pos = texture(shadow_map_pos, dir).xyz;
        vec3 sample_normal = normalize(texture(shadow_map_normal, dir).xyz);
        vec3 sample_flux = texture(shadow_map_flux, dir).xyz;
        vec3 normal = normalize(vNormal);
        float dist2_bounce = dot(vPos - sample_pos,vPos - sample_pos);
        vec3 dir_bounce = normalize(vPos - sample_pos);
        float dot1 = max(0.0, dot(sample_normal, dir_bounce));
        float dot2 = max(0.0, dot(normal, -dir_bounce));
        vec3 E = sample_flux * dot1 * dot2 / dist2_bounce;
        rsm_contribution += 1.0/64 * E;
    }
    
    color += rsm_contribution;

    FragColor = vec4(color, 1.0);
}
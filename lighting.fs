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

uniform float rnds[1024];

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


float rndPseudoGaussian(float alpha)
{
    float x=2*alpha-1;
    float y=x*x*x;
    return y;
}

float rndUniform(float alpha)
{
    float x=2*alpha-1;
    float y=x;
    return y;
}


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
    vec3 n = normalize(vNormal);
    for(int i=0;i<n_point_light;i++)
    {
        vec3 Pl = point_light[i].pos;
        vec3 Ei = point_light[i].val / dot(Pl-Ps, Pl-Ps);
        vec3 Wi = normalize(Pl-Ps);
        vec3 Wo = normalize(camera_pos-Ps);
        vec3 h = normalize(Wi + Wo);
        vec3 Ld = 1.0 / 3.14159 * Kd * Ei * max(0.0, dot(Wi, n)) * (dot(Wo, n) > 0 ? 1.0 : 0.0);
        vec3 Ls = (Ns + 2.0) / 8 / 3.14159 * Ks * Ei * pow(max(0.0, dot(h, n)), Ns)  * (dot(Wo, n) > 0 ? 1.0 : 0.0);
        float vis = 1.0;
        const int N_SAMPLE = 64;
        if(i==0)
        {
            vec3 d_receiver = (vPos - point_light[0].pos);
            float dist_receiver = length(d_receiver);
            vec3 dir_receiver = normalize(d_receiver);
            float dist_blocker = 0.0;
            float light_width = 10; // todo
            float blocker_search_radius = light_width / dist_receiver / 2;
            for(int j=0;j<N_SAMPLE;j++)
            {
                float r = (rndUniform(rnds[2*j]));
                float rx = rndPseudoGaussian(rnds[2*j+1]);
                float ry = rndPseudoGaussian(rnds[2*j+2]);
                float rz = rndPseudoGaussian(rnds[2*j+3]);
                vec3 offset = r * normalize(vec3(rx, ry, rz)) * blocker_search_radius;
                float d0 = texture(shadow_map, dir_receiver + offset).r * shadowLimit;
                dist_blocker += min(d0, dist_receiver);
            }
            dist_blocker /= N_SAMPLE;
            float shadow_radius = (dist_receiver - dist_blocker) / dist_receiver / dist_blocker * light_width;
            float shadow_bias = max(0.1 * (1.0 - dot(Wi, n)), 0.01);
            for(int j=0;j<N_SAMPLE;j++)
            {
                float r = (rndUniform(rnds[2*j]));
                float rx = rndPseudoGaussian(rnds[2*j+1]);
                float ry = rndPseudoGaussian(rnds[2*j+2]);
                float rz = rndPseudoGaussian(rnds[2*j+3]);
                vec3 offset = r * normalize(vec3(rx, ry, rz)) * shadow_radius;
                float d0 = texture(shadow_map, dir_receiver + offset).r * shadowLimit;
                if(dist_receiver - d0 > shadow_bias) vis -= 1.0 / N_SAMPLE;
            }
            // color += ((dist_receiver - dist_blocker)) * vec3(1.0, 0.0, 0.0);
        }

        color += (Ld + Ls) * vis;
    }

    // RSM 
    vec3 rsm_contribution = vec3(0.0,0.0,0.0);
    float sum_weight=0;
    for(int j=0;j<1;j++)
    for(int i=0;i<32;i++)
    {
        vec3 dist_receiver = normalize(vPos - point_light[0].pos);
        vec3 bias = rndPseudoGaussian(rnds[i*4+j]) *
            normalize(vec3(rndPseudoGaussian(rnds[i*4+1+j]), rndPseudoGaussian(rnds[i*4+2+j]), rndPseudoGaussian(rnds[i*4+3+j])));
        vec3 dir = dist_receiver + bias * 3.0;
        dir = normalize(dir);
        float weight = min(1.0, dot(dir-dist_receiver, dir-dist_receiver));
        vec3 sample_pos = texture(shadow_map_pos, dir).xyz;
        vec3 sample_normal = texture(shadow_map_normal, dir).xyz;
        vec3 sample_flux = texture(shadow_map_flux, dir).xyz;
        vec3 normal = normalize(vNormal);
        float dist2_bounce = dot(vPos - sample_pos, vPos - sample_pos) + 1e-4;
        vec3 dir_bounce = normalize(vPos - sample_pos);
        float dot1 = max(0.0, dot(sample_normal, dir_bounce));
        float dot2 = max(0.0, dot(normal, -dir_bounce));
        vec3 E = sample_flux / 3.14159 * dot1 * 1.0 / dist2_bounce;
        vec3 Ei = E * weight;
        vec3 Wi = -dir_bounce;
        vec3 Wo = normalize(camera_pos-Ps);
        vec3 h = normalize(Wi + Wo);
        vec3 Ld = 1.0 / 3.14159 * Kd * Ei * max(0.0, dot(Wi, n)) * (dot(Wo, n) > 0 ? 1.0 : 0.0);
        vec3 Ls = (Ns + 2.0) / 8 / 3.14159 * Ks * Ei * pow(max(0.0, dot(h, n)), Ns) * (dot(Wo, n) > 0 ? 1.0 : 0.0);
        rsm_contribution += Ld + Ls;
        sum_weight += weight;
    }
    rsm_contribution *= 4 * 3.14159 / sum_weight;
    color += rsm_contribution;

    FragColor = vec4(color, 1.0);
}
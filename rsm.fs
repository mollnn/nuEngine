#version 330 core

in vec2 vTex;
out vec4 FragColor;

// INPUTS
uniform sampler2D gbuf0;
uniform sampler2D gbuf1;
uniform sampler2D gbuf2;
uniform sampler2D gbuf3;
uniform sampler2D gbuf4;
uniform sampler2D gbuf5;
uniform sampler2D ao;

uniform samplerCube shadow_map;
uniform samplerCube shadow_map_pos;
uniform samplerCube shadow_map_normal;
uniform samplerCube shadow_map_flux;
uniform float shadowLimit;

uniform vec3 camera_pos;

uniform vec3 ambient;
struct PointLight 
{
    vec3 pos;
    vec3 val; // intensity
    float width;
};
uniform int n_point_light;
uniform PointLight point_light[4];

// RANDOMS
uniform sampler2D screen_rnd_tex;

layout (std140) uniform ub_common
{
    uniform float rnds[1024];
};

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
    
    int scrx = int(vTex.x * 640);
    int scry = int(vTex.y * 360);
    float scrrnd = texture(screen_rnd_tex, vTex).x;

    vec3 color = vec3(0.0);
    vec3 Ps = vPos;
    vec3 n = normalize(vNormal);

    // RSM 
    vec3 rsm_contribution = vec3(0.0,0.0,0.0);
    float sum_weight=0;
    for(int i=0;i<16;i++)
    {
        vec3 dist_receiver = normalize(vPos - point_light[0].pos);
        float cos_theta = rnds[i*3] * 2 - 1;
        float sin_theta = sqrt(1-cos_theta*cos_theta);
        float phi = rnds[i*3+1] * 3.14159 * 2;
        phi += scrrnd * 3.14159 * 2;
        float r = rndPseudoGaussian(rnds[i*3+2]);
        vec3 n = normalize(vec3(1.0));
        vec3 t = normalize(dot(vec3(1.0, 0.0, 0.0), n) > 0.5 ? cross(vec3(0.0, 1.0, 0.0), n) : cross(vec3(1.0, 0.0, 0.0), n));
        vec3 b = cross(n, t);
        vec3 bias = r * (sin_theta*cos(phi)*t + sin_theta*sin(phi)*b + cos_theta*n);

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
        vec3 Ei = E;
        vec3 Wi = -dir_bounce;
        vec3 Wo = normalize(camera_pos-Ps);
        vec3 h = normalize(Wi + Wo);
        vec3 Ld = 1.0 / 3.14159 * Kd * Ei * max(0.0, dot(Wi, n)) * (dot(Wo, n) > 0 ? 1.0 : 0.0);
        // vec3 Ls = (Ns + 2.0) / 8 / 3.14159 * Ks * Ei * pow(max(0.0, dot(h, n)), Ns) * (dot(Wo, n) > 0 ? 1.0 : 0.0);
        rsm_contribution += (Ld) * weight;
        sum_weight += weight;
    }
    rsm_contribution *= 4 * 3.14159 / sum_weight;
    color += rsm_contribution;
    FragColor = vec4(color, 1.0);
}
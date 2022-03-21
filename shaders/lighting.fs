#version 330 core

in vec2 v_texcoord;
out vec4 FragColor;

// INPUTS
uniform sampler2D gbuf0;
uniform sampler2D gbuf1;
uniform sampler2D gbuf2;
uniform sampler2D gbuf3;
uniform sampler2D gbuf4;
uniform sampler2D gbuf5;
uniform sampler2D gbuf6;
uniform sampler2D gbuf7;
uniform sampler2D ao;

uniform samplerCube shadow_map;
uniform samplerCube shadow_map_pos;
uniform samplerCube shadow_map_normal;
uniform samplerCube shadow_map_flux;
uniform float shadow_limit;

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
    vec3 g_pos = texture(gbuf0, v_texcoord).xyz;
    vec3 g_normal = texture(gbuf1, v_texcoord).xyz;
    vec3 Ka = texture(gbuf2, v_texcoord).xyz;
    vec3 Kd = texture(gbuf3, v_texcoord).xyz;
    vec3 Ks = texture(gbuf4, v_texcoord).xyz;
    float Ns = texture(gbuf5, v_texcoord).x;
    vec3 g_tangent = texture(gbuf6, v_texcoord).xyz;
    vec3 g_bitangent = texture(gbuf7, v_texcoord).xyz;
    
    int scrx = int(v_texcoord.x * 1920);
    int scry = int(v_texcoord.y * 1080);
    float scrrnd = texture(screen_rnd_tex, v_texcoord).x;

    vec3 color = ambient * Ka * (texture(ao, v_texcoord).r);
    vec3 Ps = g_pos;
    vec3 n = normalize(g_normal);
    
    // bump mapping
    
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
        if(i==0)
        {
            const int N_SAMPLE = 8;
            float shadow_bias = max(0.3 * (1.0 - dot(Wi, n)), 0.03);
            vec3 d_receiver = (g_pos - point_light[0].pos);
            float dist_receiver = length(d_receiver);
            vec3 dir_receiver = normalize(d_receiver);
            float dist_blocker = 0.0;
            float n_count = 0.0;
            float light_width = point_light[0].width; 
            // float blocker_search_radius = light_width / dist_receiver / 2;
            float blocker_search_radius = 0.02;
            for(int j=0;j<N_SAMPLE;j++)
            {
                // float r = (rndUniform(rnds[2*j]));
                // float rx = rndPseudoGaussian(rnds[2*j+1]);
                // float ry = rndPseudoGaussian(rnds[2*j+2]);
                // float rz = rndPseudoGaussian(rnds[2*j+3]);
                // vec3 offset = r * normalize(vec3(rx, ry, rz)) * blocker_search_radius;
                float phi = rnds[j*2] * 3.14159 * 2;
                phi += scrrnd * 3.14159 * 2;
                float r = pow(rnds[j*2+1], 1);
                vec3 n = normalize(dir_receiver);
                vec3 t = normalize(dot(vec3(1.0, 0.0, 0.0), n) > 0.5 ? cross(vec3(0.0, 1.0, 0.0), n) : cross(vec3(1.0, 0.0, 0.0), n));
                vec3 b = cross(n, t);
                vec3 offset = r * (cos(phi)*t + sin(phi)*b) * blocker_search_radius;
                float d0 = texture(shadow_map, dir_receiver + offset).r * shadow_limit;
                if(dist_receiver - d0 > shadow_bias)
                {
                    dist_blocker += d0;
                    n_count += 1.0;
                }
            }
            n_count += 1e-6;
            dist_blocker += 1e-6 * dist_receiver;
            dist_blocker /= n_count;
            float shadow_radius = (dist_receiver - dist_blocker) / dist_receiver / dist_blocker * light_width;
            for(int j=0;j<N_SAMPLE;j++)
            {
                // float r = (rndUniform(rnds[2*j]));
                // float rx = rndPseudoGaussian(rnds[2*j+1]);
                // float ry = rndPseudoGaussian(rnds[2*j+2]);
                // float rz = rndPseudoGaussian(rnds[2*j+3]);
                // vec3 offset = r * normalize(vec3(rx, ry, rz)) * shadow_radius;
                float phi = rnds[j*2] * 3.14159 * 2;
                phi += scrrnd * 3.14159 * 2;
                float r = pow(rnds[j*2+1], 1);
                vec3 n = normalize(dir_receiver);
                vec3 t = normalize(dot(vec3(1.0, 0.0, 0.0), n) > 0.5 ? cross(vec3(0.0, 1.0, 0.0), n) : cross(vec3(1.0, 0.0, 0.0), n));
                vec3 b = cross(n, t);
                vec3 offset = r * (cos(phi)*t + sin(phi)*b) * shadow_radius;
                float d0 = texture(shadow_map, dir_receiver + offset).r * shadow_limit;
                if(dist_receiver - d0 > shadow_bias) vis -= 1.0 / N_SAMPLE;
            }
            // color += ((dist_receiver - dist_blocker)) * vec3(1.0, 0.0, 0.0);
        }
        color += (Ld + Ls) * vis;
    }

    FragColor = vec4(color, 1.0);
}
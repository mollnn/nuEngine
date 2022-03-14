#version 330 core

in vec3 vPos;
in vec3 vNormal;
in vec2 vTex;
in vec4 vLPos;

out vec4 FragColor;

uniform vec3 color_diffuse;
uniform vec3 color_specular;
uniform int usetex_diffuse;
uniform int usetex_specular;
uniform float shininess;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform samplerCube shadow_map;

uniform float shadowLimit;

struct PointLight 
{
    vec3 pos;
    vec3 val; // intensity
};

uniform int n_point_light;
uniform PointLight point_light[4];

void main()
{
    vec3 color;
    float Ns = shininess;
    for(int i=0;i<n_point_light;i++)
    {
        vec3 Pl = point_light[i].pos;
        vec3 Ps = vPos;
        vec3 n = vNormal;
        vec3 Ei = point_light[i].val / dot(Pl-Ps, Pl-Ps);
        vec3 Wi = normalize(Pl-Ps);
        vec3 Kd = texture(texture_diffuse1, vTex).xyz;
        if(usetex_diffuse==0) Kd=color_diffuse;
        vec3 Ld = 1.0 / 3.14159 * Kd * Ei * max(0.0, dot(Wi, n));
        vec3 Ks = texture(texture_specular1, vTex).xyz;
        if(usetex_specular==0) Ks=color_specular;
        vec3 Ls = (Ns + 2.0) / 8 / 3.14159 * Ks * Ei * pow(max(0.0, dot(Wi, n)), Ns);
        float vis = 1.0;
        if(i==0)
        {
            vec3 dp = vPos - point_light[0].pos;
            float d0 = texture(shadow_map, dp).r * shadowLimit;
            float d = length(dp);
            vis = d- d0 > 0.05 ? 0.0 : 1.0;
            // color = vec3(d0 -d + 0.5, d-d0+0.5, d0 -d + 0.5);
        }
        color += (Ld + Ls) * vis;
    }
    FragColor = vec4(color, 1.0);
}
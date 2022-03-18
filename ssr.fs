#version 330 core

in vec2 vTex;

out vec4 FragColor;

uniform sampler2D gbuf0;
uniform sampler2D gbuf1;
uniform sampler2D gbuf2;
uniform sampler2D gbuf3;
uniform sampler2D gbuf4;
uniform sampler2D gbuf5;
uniform sampler2D film;
uniform sampler2D screen_rnd_tex;

uniform float rnds[1024];

uniform vec3 camera_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float near;
uniform float far;



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


float linearDepth(float depth)
{
    float z = depth;
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

vec3 worldToScreen(vec3 w)
{
    vec4 t = projection * view * vec4(w, 1.0);
    return (t.xyz / t.w + 1) / 2;
}


vec3 intersection(vec3 o, vec3 d)
{
    float s=0.01;
    float a=0.005;
    vec3 p=o;
    for(int i=0;i<32;i++)
    {
        p+=d*s;
        s+=a;
        vec3 p_screen = worldToScreen(p);
        float cursor_depth = p_screen.z * 2 - 1;
        float record_depth = texture(gbuf0, p_screen.xy).a;
        if(cursor_depth > 1e-3 + record_depth)
        {
            return p;
        }
    }
    return vec3(2e18);
}


void main()
{
    vec3 vPos = texture(gbuf0, vTex).xyz;
    vec3 vNormal = texture(gbuf1, vTex).xyz;
    vec3 Ka = texture(gbuf2, vTex).xyz;
    vec3 Kd = texture(gbuf3, vTex).xyz;
    vec3 Ks = texture(gbuf4, vTex).xyz;
    float Ns = texture(gbuf5, vTex).x;
    
    float scrrnd = texture(screen_rnd_tex, vTex).x;

    vec3 color = texture(film, vTex.xy).xyz;

    vec3 p = vPos;
    vec3 n = normalize(vNormal);
    vec3 wo = normalize(camera_pos - p);

    int N_SAMPLE = 4;
    for(int i=0;i<N_SAMPLE;i++)
    {
        vec3 ax1 = normalize(dot(vec3(1.0, 0.0, 0.0), n) > 0.5 ? cross(vec3(0.0, 1.0, 0.0), n) : cross(vec3(1.0, 0.0, 0.0), n));
        vec3 ax2 = cross(n, ax1);
        float r1 = rnds[2*i+0];
        float r2 = rnds[2*i+1];
        r1 += scrrnd;
        if(r1>=1.0) r1-=1.0;
        r2 += scrrnd;
        if(r2>=1.0) r2-=1.0;
        float cos_theta = pow(r1, 1.0 / (Ns + 2.0));
        float sin_theta = sqrt(1 - cos_theta);
        float phi = 2.0 * 3.14159 * r2;
        vec3 h = cos_theta * n + sin_theta * cos(phi) * ax1 + sin_theta * sin(phi) * ax2;
        float pdf = (Ns + 2.0) / 8 / 3.14159 * pow(max(0.0, dot(h, n)), Ns + 1.0);
        vec3 wi = reflect(-wo, h);
        vec3 hitpos = intersection(p, wi);
        if(length(hitpos)<1e18)
        {
            vec3 screen_pos = worldToScreen(hitpos);
            if (screen_pos.x > 0.0 && screen_pos.y > 0.0 && screen_pos.x < 1.0 && screen_pos.y < 1.0)
            {
                vec3 Pl = hitpos;
                vec3 Ps = vPos;
                vec3 Li = texture(film, screen_pos.xy).xyz;
                vec3 Wi = normalize(Pl-Ps);
                vec3 Wo = normalize(camera_pos-Ps);
                vec3 h = normalize(Wi + Wo);
                vec3 Ld = 1.0 / 3.14159 * Kd * Li * max(0.0, dot(Wi, n)) * (dot(Wo, n) > 0 ? 1.0 : 0.0);
                vec3 Ls = (Ns + 2.0) / 8 / 3.14159 * Ks * Li * pow(max(0.0, dot(h, n)), Ns)  * (dot(Wo, n) > 0 ? 1.0 : 0.0);
                color += (Ls + Ld) / N_SAMPLE / pdf;
                // color = hitpos;
            }
        }
    }

        //     vec3 p_screen = worldToScreen(p);
        // float cursor_depth = p_screen.z * 2 - 1;
        // float record_depth = texture(gbuf0, p_screen.xy).a;
        // color = vec3(cursor_depth, record_depth, cursor_depth) * 0.5+0.5;
    FragColor = vec4(color, 1.0);
}
#version 330 core

in vec2 v_texcoord;

out vec4 FragColor;

uniform sampler2D gbuf0;
uniform sampler2D gbuf1;
uniform sampler2D gbuf2;
uniform sampler2D gbuf3;
uniform sampler2D gbuf4;
uniform sampler2D gbuf5;
uniform sampler2D film;
uniform sampler2D screen_rnd_tex;

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
    float a=0.02;
    vec3 p=o;
    float thickness=0.3;
    for(int i=0;i<16;i++)
    {
        p+=d*s;
        s+=a;
        vec3 p_screen = worldToScreen(p);
        float cursor_depth = p_screen.z * 2 - 1;
        float record_depth = texture(gbuf0, p_screen.xy).a;
        if(cursor_depth > 1e-3 + record_depth && linearDepth(cursor_depth) - linearDepth(record_depth) < thickness + s)
        {
            return p;
        }
        if(cursor_depth > 1e-3 + record_depth)
        {
            return vec3(2e18);
        }
    }
    return vec3(2e18);
}


void main()
{
    vec3 v_pos = texture(gbuf0, v_texcoord).xyz;
    vec3 v_normal = texture(gbuf1, v_texcoord).xyz;
    vec3 Ka = texture(gbuf2, v_texcoord).xyz;
    vec3 Kd = texture(gbuf3, v_texcoord).xyz;
    vec3 Ks = texture(gbuf4, v_texcoord).xyz;
    float Ns = texture(gbuf5, v_texcoord).x;
    
    float scrrnd = texture(screen_rnd_tex, v_texcoord).x;

    vec3 color = vec3(0.0);

    vec3 p = v_pos;
    vec3 n = normalize(v_normal);
    vec3 wo = normalize(camera_pos - p);

    int N_SAMPLE = 1;
    for(int i=0;i<N_SAMPLE;i++)
    {
        vec3 h, f_pdf;
        if(Ns<1000)
        {
            // glossy
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
            h = cos_theta * n + sin_theta * cos(phi) * ax1 + sin_theta * sin(phi) * ax2;
            f_pdf = Ks;
        }
        else
        {
            // specular
            h = n;
            f_pdf = Ks;
        }
        vec3 wi = reflect(-wo, h);
        vec3 hitpos = intersection(p, wi);
        if(length(hitpos)<1e18)
        {
            vec3 screen_pos = worldToScreen(hitpos);
            if (screen_pos.x > 0.0 && screen_pos.y > 0.0 && screen_pos.x < 1.0 && screen_pos.y < 1.0)
            {
                vec3 Pl = hitpos;
                vec3 Ps = v_pos;
                vec3 Li = texture(film, screen_pos.xy).xyz;
                vec3 Wi = normalize(Pl-Ps);
                vec3 Wo = normalize(camera_pos-Ps);
                vec3 h = normalize(Wi + Wo);
                // vec3 Ld = 1.0 / 3.14159 * Kd * Li * max(0.0, dot(Wi, n)) * (dot(Wo, n) > 0 ? 1.0 : 0.0);
                vec3 Ls = f_pdf * (dot(Wo, n) > 0 ? 1.0 : 0.0) * Li;
                color += (Ls) / N_SAMPLE;
                // color = hitpos;
            }
        }
    }

    FragColor = vec4(color, 1.0);
}
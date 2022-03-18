#version 330 core

in vec2 vTex;

out vec4 color;

uniform sampler2D gbuf0;
uniform sampler2D gbuf1;
uniform sampler2D gbuf2;
uniform sampler2D gbuf3;
uniform sampler2D gbuf4;
uniform sampler2D gbuf5;
uniform sampler2D screen_rnd_tex;

uniform float near;
uniform float far;

uniform float rnds[1024];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

float linearDepth(float depth)
{
    float z = depth;
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

float rndPseudoGaussian(float alpha)
{
    float x=2*alpha-1;
    float y=x*x*x;
    return x*0.5+0.5;
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
    float vDepth = texture(gbuf0, vTex).a;
    float linear_depth = linearDepth(vDepth) / far;
    vec3 vNormal = texture(gbuf1, vTex).xyz;

    float occ=1;
    float radius = 0.5;
    int N_SAMPLE = 16;

    int scrx = int(vTex.x * 640);
    int scry = int(vTex.y * 360);
    int scrid = (scrx % 16) * 16 + scry % 16;
    float scrrnd = texture(screen_rnd_tex, vTex).x;

    for(int i=0;i<N_SAMPLE;i++)
    {
        float cos_theta = rnds[i*3];
        float sin_theta = sqrt(1-cos_theta*cos_theta);
        float phi = rnds[i*3+1] * 3.14159 * 2;
        phi += scrrnd * 3.14159 * 2;
        float r = pow(rnds[i*3+2], 3);
        vec3 n = normalize(vNormal);
        vec3 t = normalize(dot(vec3(1.0, 0.0, 0.0), n) > 0.5 ? cross(vec3(0.0, 1.0, 0.0), n) : cross(vec3(1.0, 0.0, 0.0), n));
        vec3 b = cross(n, t);
        vec3 kernel = r * (sin_theta*cos(phi)*t + sin_theta*sin(phi)*b + cos_theta*n);
        kernel *= radius;
        vec3 sample_pos=vPos+kernel;
        vec4 sample_pos_ss = (projection*view*vec4(sample_pos,1.0));
        vec2 sample_xy = sample_pos_ss.xy/sample_pos_ss.w*0.5+0.5;
        float sample_d = (texture(gbuf0, sample_xy).a);
        float sample_z = (sample_pos_ss.z/sample_pos_ss.w);
        if(sample_d+1e-3<sample_z)
        {
            float sm = smoothstep(0.0, 1.0, radius / (linearDepth(sample_z) - linearDepth(sample_d)));
            occ-=1.0/N_SAMPLE*sm;
        }
    }

    color = vec4(occ,occ,occ, 1.0);
}
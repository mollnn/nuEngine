#version 330 core

in vec2 vTex;

out vec4 color;

uniform sampler2D gbuf0;
uniform sampler2D gbuf1;
uniform sampler2D gbuf2;
uniform sampler2D gbuf3;
uniform sampler2D gbuf4;
uniform sampler2D gbuf5;

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


void main()
{
    vec3 vPos = texture(gbuf0, vTex).xyz;
    float vDepth = texture(gbuf0, vTex).a;
    float linear_depth = linearDepth(vDepth) / far * 10;
    vec3 vNormal = texture(gbuf1, vTex).xyz;

    float occ=1;
    for(int i=0;i<32;i++)
    {
        vec3 kernel = vec3(rndPseudoGaussian(rnds[i*3]), rndPseudoGaussian(rnds[i*3+1]), rndPseudoGaussian(rnds[i*3+2]));
        if(dot(kernel,vNormal)<0) kernel*=-1;
        kernel *= 0.5;
        vec3 sample_pos=vPos+kernel;
        vec4 sample_pos_ss = (projection*view*vec4(sample_pos,1.0));
        vec2 sample_xy = sample_pos_ss.xy/sample_pos_ss.w*0.5+0.5;
        float sample_d = texture(gbuf0, sample_xy).a;
        float sample_z = sample_pos_ss.z/sample_pos_ss.w;
        if(sample_d<sample_z)
        {
            occ-=1.0/32;
        }
    }

    color = vec4(occ,occ,occ, 1.0);
}
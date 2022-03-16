#version 330 core

in vec3 vPos;
in vec3 vNormal;
in vec2 vTex;
layout (location = 0) out vec3 gPos;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gFlux;
uniform vec3 lightInt;
uniform vec3 lightPos;
uniform float shadowLimit;

uniform vec3 color_diffuse;
uniform int usetex_diffuse;
uniform sampler2D texture_diffuse1;

void main()
{
    float lightDist = length(lightPos - vPos) / shadowLimit;
    gl_FragDepth = lightDist;
    gPos = vPos;
    gNormal = vNormal;

    vec3 n = vNormal;

    vec3 Kd = texture(texture_diffuse1, vTex).xyz;
    if(usetex_diffuse==0) Kd=color_diffuse;

    vec3 Pl = lightPos;
    vec3 Ps = vPos;
    vec3 Ei = lightInt / dot(Pl-Ps, Pl-Ps);
    vec3 Wi = normalize(Pl - Ps);
    vec3 Ld = 1.0 / 3.14159 * Kd * Ei * max(0.0, dot(Wi, n));
    float A = 1.0 / dot(Pl-Ps, Pl-Ps) / max(1e-6, dot(Wi, n));   // w0 is eliminated
    vec3 Phi = Ld * 3.14159 * A;

    gFlux = Ld;
}
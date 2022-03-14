#version 330 core

in vec3 fragPos;
uniform vec3 lightPos;
uniform float shadowLimit;

void main()
{
    float lightDist = length(lightPos - fragPos) / shadowLimit;
    gl_FragDepth = lightDist;
}
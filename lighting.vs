#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

out vec3 vPos;
out vec3 vNormal;
out vec2 vTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    vTex=aTex;
}
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

uniform mat4 lvp;
uniform mat4 model;

out vec3 fragPos;

void main()
{
    gl_Position = lvp * model * vec4(aPos, 1.0f);
    fragPos = (model * vec4(aPos, 1.0f)).xyz;
}
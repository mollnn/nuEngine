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
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    mat4 model_o = transpose(inverse(model));
    vPos=(model * vec4(aPos, 1.0)).xyz;
    vTex=aTex;
    vNormal=(model_o*vec4(aNormal, 1.0)).xyz;
}
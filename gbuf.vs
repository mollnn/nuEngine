#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;

out vec3 v_pos;
out vec3 v_normal;
out vec2 v_texcoord;
out vec4 v_pos4;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    v_pos4 = gl_Position;
    mat4 model_o = transpose(inverse(model));
    v_pos=(model * vec4(a_pos, 1.0)).xyz;
    v_texcoord=a_texcoord;
    v_normal=(model_o*vec4(a_normal, 1.0)).xyz;
}
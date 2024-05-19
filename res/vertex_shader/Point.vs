#version 330 core

layout(location=0) in vec3 v_pos;
layout(location=1) in vec3 v_color;
uniform mat4 uni_transform;

out vec3 f_color;

void main()
{
    f_color=v_color;
    gl_Position=uni_transform*vec4(v_pos,1.0);
    gl_Position/=gl_Position.w;
}
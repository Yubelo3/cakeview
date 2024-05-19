#version 330 core

layout(location=0) in vec3 v_pos;
layout(location=1) in vec3 v_normal;

out vec3 f_pos;
out vec3 f_normal;

uniform mat4 uni_transform;  //MVP
uniform mat4 uni_modelTransform;  //M

void main()
{
    gl_Position=uni_transform*vec4(v_pos,1.0);
    gl_Position/=gl_Position.w;

    f_pos=(uni_modelTransform*vec4(v_pos,1.0)).xyz;
    f_normal=mat3(transpose(inverse(uni_modelTransform)))*v_normal;
}
#version 330 core  //版本号，核心模式
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aColor;

out vec4 color;
uniform mat4 transform;

//vertex_shader任务：给出一个顶点的NDC坐标，传到gl_Position变量
void main()
{
    color=vec4(aColor,1.0);

    //输出交给预定义的vec4 gl_Position变量
    gl_Position=vec4(aPos,1.0);
    gl_Position=transform*gl_Position;
    gl_Position/=gl_Position.w;
    gl_Position.z*=1.0;
}
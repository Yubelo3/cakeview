#version 330 core  //版本号，核心模式
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aColor;
layout(location=2) in vec2 aTexCoord;

out vec4 vertexColor;
out vec2 texCoord;

uniform float alpha;
uniform mat4 transform;

//vertex_shader任务：给出一个顶点的NDC坐标，传到gl_Position变量
void main()
{
    vertexColor=vec4(aColor.xy,alpha,1.0);
    texCoord=aTexCoord;

    //输出交给预定义的vec4 gl_Position变量
    gl_Position=vec4(aPos,1.0);
    gl_Position=transform*gl_Position;
    gl_Position/=gl_Position.w;
}
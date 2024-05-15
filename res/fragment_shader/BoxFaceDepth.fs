#version 330 core

in vec4 vertexColor;
in vec2 texCoord;


out vec4 FragColor;  //指定输出

uniform sampler2D texture0;  //内建数据类型，采样器
uniform sampler2D texture1;  //内建数据类型，采样器

void main()
{
    FragColor=mix(texture(texture0,texCoord),texture(texture1,texCoord),0.5)*vertexColor;  //texture是GLSL内建函数
    FragColor=FragColor*0.0001+vec4(gl_FragCoord.zzz,1.0);
}
#version 330 core

in type in_var_name;
in type in_var_name;
//对于顶点着色器，最多可以有16个4分量的输入变量（顶点属性）
//顶点着色器还可以有 layout(location=0) in type in_var_name这种顶点属性数据传递

out type out_var_name;
//对于片段着色器，需要一个vec4输出颜色。如果没有，会渲染成黑色或白色。

uniform type uniform_name;

//基本类型: int/float/double/uint/bool
//容器（向量和矩阵）类型: vecn/bvecn/ivecn/uvecn/dvecn
//容器通过.x .y .z .w来获取分量
//可以通过.xyz来一次获取多个分量，可以通过.xyxy来通过分量组合成新的向量

//着色器向后续着色器传递
//例如，在顶点着色器中写out vec4 vertexColor
//在片段着色器中写in vec4 vertexColor
//类型和名字都对上时，输入输出就会链接

//CPU向着色器传递: uniform
//uniform声明后可以被任意着色器在任意阶段访问
//


int main()
{
    //processing...

    out_var_name=processed;    
}
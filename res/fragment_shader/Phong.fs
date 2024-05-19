#version 330 core

in vec3 f_pos;
in vec3 f_normal;

uniform vec3 uni_lightPos;
uniform vec3 uni_lightColor;
uniform vec3 uni_objectColor;

out vec4 FragColor;

void main()
{
    float ka=0.01;
    float kd=0.5;
    float ks=0.6;
    float p=3.0;
    
    float d=distance(uni_lightPos,f_pos);
    vec3 lightColor=uni_lightColor/(d*d);

    vec3 l=normalize(uni_lightPos-f_pos);
    vec3 v=normalize(-f_pos);
    vec3 h=normalize(l+v);

    //ambient
    vec3 ambient=ka*uni_lightColor;

    //diffuse
    float cosTheta=max(dot(l,f_normal),0.0);
    vec3 diffuse=kd*cosTheta*lightColor;

    //specular
    float cosPhi=max(dot(h,f_normal),0.0);
    vec3 specular=ks*pow(cosPhi,p)*lightColor;

    vec3 result=(ambient+diffuse+specular)*uni_objectColor;

    FragColor=vec4(result,1.0);
}
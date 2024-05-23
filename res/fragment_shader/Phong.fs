#version 330 core
#define MAX_N_LIGHTS 4

in vec3 f_pos;
in vec3 f_normal;
uniform int uni_nlights;

struct PointLight
{
    vec3 pos;
    vec3 color;
};
uniform PointLight uni_lights[MAX_N_LIGHTS];

struct Material
{
    float ka;
    float kd;
    float ks;
    float p;
};

const Material material=Material(0.01,0.5,0.6,3.0);

vec3 shade(PointLight light)
{
    float d=distance(light.pos,f_pos);
    vec3 lightColor=light.color/(d*d);
    vec3 l=normalize(light.pos-f_pos);
    vec3 v=normalize(-f_pos);
    vec3 h=normalize(l+v);

    //ambient
    vec3 ambient=material.ka*lightColor;

    //diffuse
    float cosTheta=max(dot(l,f_normal),0.0);
    vec3 diffuse=material.kd*cosTheta*lightColor;

    //specular
    float cosPhi=max(dot(h,f_normal),0.0);
    vec3 specular=material.ks*pow(cosPhi,material.p)*lightColor;

    return ambient+diffuse+specular;
}

out vec4 FragColor;

void main()
{
    vec3 result=vec3(0.0,0.0,0.0);
    for(int i=0;i<uni_nlights;i++)
        result+=shade(uni_lights[i]);
    FragColor=vec4(result,1.0);
}
#version 330 core

struct DirLight{
  vec3  f_Dir;
  vec3  f_Ia;
  vec3  f_Id;
  vec3  f_Is;
};

struct PointLight{
    vec3 f_LightPos;
    vec3 f_Ia;
    vec3 f_Id;
    vec3 f_Is;
    float f_Const;
    float f_Linear;
    float f_Quadratic;
};

struct Material{
    sampler2D f_Ka;
    sampler2D f_Kd;
    sampler2D f_Ks;
    float f_Shinness;
};

in vec2 f_TexCoord;
in vec3 f_FragPos;
in vec3 f_Normal;

uniform vec3 f_EyePos;
uniform DirLight f_DirLight;
uniform PointLight f_PointLight;
uniform Material f_Material;

out vec4 o_Color;


float near = 0.1; 
float far  = 100.0; 

vec3 CalculateDirLight(vec3 normal,vec3 eyeDir,DirLight dirLight){
    vec3 La=vec3(texture(f_Material.f_Ka,f_TexCoord))*dirLight.f_Ia;
    vec3 Ld=vec3(texture(f_Material.f_Kd,f_TexCoord))*dirLight.f_Id*max(0,dot(dirLight.f_Dir,normal));
    vec3 Ls=vec3(texture(f_Material.f_Ks,f_TexCoord))*dirLight.f_Is*pow(max(0,dot(normalize((eyeDir+dirLight.f_Dir)*0.5f),normal)),f_Material.f_Shinness);
    return La+Ld+Ls;
}

vec3 CalculatePointLight(vec3 normal,vec3 eyeDir,vec3 fragPos,PointLight pointLight){
    vec3 lightDir=normalize(pointLight.f_LightPos-fragPos);
    float dis=distance(pointLight.f_LightPos,fragPos);
    float disAttenuation=1/(pointLight.f_Const+dis*pointLight.f_Linear+dis*dis*pointLight.f_Quadratic);
    vec3 La=disAttenuation*vec3(texture(f_Material.f_Ka,f_TexCoord))*pointLight.f_Ia;
    vec3 Ld=disAttenuation*vec3(texture(f_Material.f_Kd,f_TexCoord))*pointLight.f_Id*max(0,dot(lightDir,normal));
    vec3 Ls=disAttenuation*vec3(texture(f_Material.f_Ks,f_TexCoord))*pointLight.f_Is*pow(max(0,dot(normalize((eyeDir+lightDir)*0.5f),normal)),f_Material.f_Shinness);
    return La+Ld+Ls;
}

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main(){
    vec3 normalDir=normalize(f_Normal);
    vec3 eyeDir=normalize(f_EyePos-f_FragPos);
    
    vec3 color = CalculateDirLight(normalDir,eyeDir,f_DirLight);
    color+=CalculatePointLight(normalDir,eyeDir,f_FragPos,f_PointLight);
    
    float depth=LinearizeDepth(gl_FragCoord.z)/far;
    
    float test = gl_FragCoord.z/2.0f;
    o_Color=vec4(color,1.0f);//*vec4(texture(f_Texture,f_TexCoord));//vec4(La+Ld+Ls,1.0f)*
   
}
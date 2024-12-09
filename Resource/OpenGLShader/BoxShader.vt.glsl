#version 330 core
layout (location=0) in vec3 v_Pos;
layout (location=1) in vec3 v_Normal;
layout (location=2) in vec2 v_TexCoord;

uniform mat4 v_Model;
uniform mat4 v_ViewProj;

out vec2 f_TexCoord;
out vec3 f_FragPos;
out vec3 f_Normal;

void main(){
    gl_Position=v_ViewProj*v_Model*vec4(v_Pos,1.0f);
    f_FragPos=vec3(v_Model*vec4(v_Pos,1.0f));
    f_TexCoord=v_TexCoord;
    f_Normal=vec3(transpose(inverse(v_Model))*vec4(v_Pos,1.0f));
}
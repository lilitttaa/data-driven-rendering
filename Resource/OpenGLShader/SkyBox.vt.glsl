#version 330 core
layout (location=0) in vec3 v_Pos;

uniform mat4 v_ViewProj;
out vec3 f_TexCoord;

void main(){
    vec4 pos = v_ViewProj*vec4(v_Pos,1.0f);
    gl_Position=pos.xyww;
    f_TexCoord = v_Pos;
}
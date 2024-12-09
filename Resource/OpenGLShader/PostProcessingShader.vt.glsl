#version 330 core
layout (location=0) in vec3 v_Pos;
layout (location=1) in vec3 v_Normal;
layout (location=2) in vec2 v_TexCoord;

out vec2 f_TexCoord;
void main(){
    gl_Position=vec4(v_Pos,1.0f);
    f_TexCoord=v_TexCoord;
}
#version 330 core
layout (location=0) in vec2 v_Pos;
layout (location=1) in vec2 v_TexCoord;
uniform mat4 v_TransformMat;
out vec2 f_TexCoord;

void main(){
    gl_Position=v_TransformMat*vec4(v_Pos,0.0f,1.0f);
    f_TexCoord=v_TexCoord;
}
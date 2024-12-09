#version 330 core
layout (location=0) in vec3 v_Pos;
uniform mat4 v_Model;
uniform mat4 v_ViewProj;
void main(){
    gl_Position=v_ViewProj*v_Model*vec4(v_Pos,1.0f);
}
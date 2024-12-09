#version 330 core

uniform vec3 f_LightColor;
out vec4 o_Color;
void main(){
    o_Color=vec4(f_LightColor,1.0f);
}
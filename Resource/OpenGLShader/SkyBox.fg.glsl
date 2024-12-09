#version 330 core

uniform samplerCube f_cubeMap;

in vec3 f_TexCoord;
out vec4 o_Color;

void main(){
    o_Color = texture(f_cubeMap,f_TexCoord);
}
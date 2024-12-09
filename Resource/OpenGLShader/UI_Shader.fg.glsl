#version 330 core
uniform sampler2D f_Texture; 
uniform vec4 f_Color;


in vec2 f_TexCoord;
out vec4 o_Color;

void main(){
    o_Color=texture(f_Texture,f_TexCoord)*f_Color;// vec4(result,1.0f);
}
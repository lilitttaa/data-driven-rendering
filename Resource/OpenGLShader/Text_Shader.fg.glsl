#version 330 core
uniform sampler2D f_Texture; 
uniform vec3 f_Color;

in vec2 f_TexCoord;
out vec4 o_Color;

void main(){
    o_Color=vec4(1,1,1,texture(f_Texture,f_TexCoord).r)*vec4(f_Color,1.0f);// vec4(result,1.0f);
}
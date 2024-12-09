#version 330 core

uniform sampler2D f_Texture;

in vec2 f_TexCoord;
out vec4 o_Color;

vec3 Inversion(){
    return vec3(1.0 - texture(f_Texture, f_TexCoord));
}

vec3 Grayscale(){
    vec4 fragColor = texture(f_Texture,f_TexCoord);
    float averageColor = (fragColor.r,fragColor.g,fragColor.b)/3.0f;
    return vec3(averageColor,averageColor,averageColor);
}

const float offset = 1.0/300.0f;
vec2 offsets[9] = vec2[](
    vec2(-offset,  offset), // 左上
    vec2( 0.0f,    offset), // 正上
    vec2( offset,  offset), // 右上
    vec2(-offset,  0.0f),   // 左
    vec2( 0.0f,    0.0f),   // 中
    vec2( offset,  0.0f),   // 右
    vec2(-offset, -offset), // 左下
    vec2( 0.0f,   -offset), // 正下
    vec2( offset, -offset)  // 右下
);
vec3 Convolution(float[9] kernel){
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(f_Texture, f_TexCoord.xy + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    return col;
}

vec3 Sharpen(){
    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
    return Convolution(kernel);
}

vec3 Blur(){
    float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
    );
    return Convolution(kernel);
}

vec3 EdgeDetection(){
    float kernel[9] = float[](
    1, 1, 1,
    1, -8, 1,
    1, 1, 1  
    );
    return Convolution(kernel);
}

void main(){
    //o_Color= texture(f_Texture, f_TexCoord);
    if(f_TexCoord.x>0.5){
        o_Color= vec4(Sharpen(),1.0f);
    }
    else{
        o_Color= texture(f_Texture, f_TexCoord);
    }
    
}
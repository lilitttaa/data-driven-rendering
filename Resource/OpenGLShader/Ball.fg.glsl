#version 330 core
in vec3 FragPos;
out vec4 FragColor;

// 0 - 800, 0 - 600

uniform vec2 ballPos;
uniform float ballRadius;

void DrawCircle(float x, float y, float r)
{
   if(pow(FragPos.x-x,2)+pow(FragPos.y-y,2)<r*r)
   {
       FragColor = vec4(0.0,0.0,0.0,1.0);
   }
}

void main()
{
    FragColor = vec4(1.0,1.0,1.0,1.0);
    
    DrawCircle(ballPos.x,ballPos.y,ballRadius);
}
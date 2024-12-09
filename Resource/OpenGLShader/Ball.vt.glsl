#version 330 core
layout(location = 0) in vec3 aPos;
out vec3 FragPos;

int WIDTH = 800;
int HEIGHT = 600;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    // 换算到屏幕空间坐标
    FragPos.x = (aPos.x + 1.0) * WIDTH / 2.0;
    FragPos.y = HEIGHT - (aPos.y + 1.0) * HEIGHT / 2.0;
}
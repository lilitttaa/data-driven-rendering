#version 330 core
#define FRAGMENT
#if defined VERTEX
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
        #endif // VERTEX
    
        #if defined FRAGMENT
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
        #endif // FRAGMENT
    
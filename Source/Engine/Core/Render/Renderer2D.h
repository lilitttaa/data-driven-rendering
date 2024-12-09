#pragma once

#include "Core.h"
#include "Font.h"
#include "Shader.h"
#include "Texture2D.h"
#include "VertexArray.h"
#include "UI/Canvas.h"

namespace ST
{
class Font;

struct Brush;

class AppWindow;
    class Renderer2D
    {
    public:
        Renderer2D(AppWindow* appWindow);
        void DrawQuad(const Rect& rect,const Brush& brush);
        void DrawPoint(glm::vec2&& pos,float size,glm::vec3 color);
        void DrawFrame(const Rect& rect,glm::vec3 color);
        void DrawLine(glm::vec2 pos1,glm::vec2 pos2,float size,glm::vec3 color);
        void DrawSingleLineText(glm::vec2 pos,glm::vec3 color,float scale,const ST_STRING& text);
        void DrawSingleChar(Rect&& rect,ST_FONT_CHAR c);
    private:
        glm::mat4 CreateTransformMat(const Rect& rect);
        AppWindow* _appWindow;
        ST_REF<VertexArray> _vertexArray;
        ST_REF<VertexArray> _textVertexArray;
        ST_REF<Shader> _shader;
        ST_REF<Shader> _texShader;
        ST_REF<Texture2D> _texture;
        ST_REF<Font> _font;
    };
}

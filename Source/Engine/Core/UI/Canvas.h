#pragma once
#include "Core.h"
#include "Rect.h"

namespace ST {
struct Rect;

struct Event;

class Renderer2D;

class Widget;

class AppWindow;

class Canvas //:public std::enable_shared_from_this<Canvas>
{
public:
	Canvas(Rect&& rect);

	Rect& GetRect() { return _rect; }

	void Draw(const ST_REF<Renderer2D>& renderer);

	void AddChild(ST_REF<Widget> child, int idx = -1);

	void OnEvent(const AppWindow& appWindow, const Event& e);

	/*virtual void GetCanvasSize(float& xSize,float& ySize);*/
protected:
	Rect _rect;

	ST_VECTOR<ST_REF<Widget>> _children;
};
}

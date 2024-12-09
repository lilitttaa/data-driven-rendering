#pragma once

#include "Core.h"
#include "Rect.h"

#include "Event/Event.h"

namespace ST {
struct Rect;

class Canvas;

class Renderer2D;

class Widget {
public:
	Widget(Widget* parent, Canvas* canvas): _rect({0, 0}, {0, 0}),
		_parent(parent), _canvas(canvas), _cachedHovered(false) { }

	Widget(const Rect& rect, Widget* parent, Canvas* canvas): _rect(rect),
		_parent(parent), _canvas(canvas) { }

	virtual ~Widget() {}

	bool IsInWidget(float x, float y);

	Rect GetLocalRect() const;

	void SetLocalRect(Rect&& rect);

	Rect GetGlobalRect() const;

	void AddChild(ST_REF<Widget> child, int idx = -1);

	virtual void Draw(const ST_REF<Renderer2D>& render2D) {}

	virtual void OnMouseButtonPressed(const MouseButtonPressedEvent& e);

	virtual void OnMouseButtonReleased(const MouseButtonReleasedEvent& e);

	virtual void OnMouseHovered(const MouseMovedEvent& e);

	virtual void OnMouseLeft(const MouseMovedEvent& e);

protected:
	Rect _rect;

	ST_VECTOR<ST_REF<Widget>> _children;

	Widget* _parent;

	Canvas* _canvas;

public:
	bool _cachedHovered;
};
}

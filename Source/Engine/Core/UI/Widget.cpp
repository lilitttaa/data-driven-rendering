#include "Widget.h"

#include "Canvas.h"
#include "Rect.h"

bool ST::Widget::IsInWidget(float x, float y) { return GetGlobalRect().IsInRect(x, y); }

ST::Rect ST::Widget::GetLocalRect() const { return _rect; }

void ST::Widget::SetLocalRect(Rect&& rect) { _rect = rect; }

ST::Rect ST::Widget::GetGlobalRect() const {
	if (_canvas) {
		const Rect& canvasRect = _canvas->GetRect();
		return {
			canvasRect._pos.x + _rect._pos.x,
			canvasRect._pos.y + _rect._pos.y,
			_rect._size.x,
			_rect._size.y
		};
	}
	return Rect();
}

void ST::Widget::AddChild(ST_REF<Widget> child, int idx) {
	if (idx == -1) { _children.push_back(child); }
	else { _children.insert(_children.begin() + idx, child); }
}

void ST::Widget::OnMouseButtonPressed(const MouseButtonPressedEvent& e) {
	//ST_LOG("OnMouseButtonPressed\n");
}

void ST::Widget::OnMouseButtonReleased(const MouseButtonReleasedEvent& e) {
	//ST_LOG("OnMouseButtonReleased\n");
}

void ST::Widget::OnMouseHovered(const MouseMovedEvent& e) {
	_cachedHovered=true;
	//ST_LOG("OnMouseButtonHovered\n");
}

void ST::Widget::OnMouseLeft(const MouseMovedEvent& e) {
	_cachedHovered=false;
	//ST_LOG("OnMouseLeft\n");
}

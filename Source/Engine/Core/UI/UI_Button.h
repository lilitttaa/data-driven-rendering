#pragma once
#include "Brush.h"
#include "vec3.hpp"
#include "Widget.h"
#include "Render/Texture2D.h"

namespace ST {
enum class ButtonMode {
	NORMAL = 0, PRESSED, HOVERED, DISABLED
};

using MouseButtonPressedCallback = void*(const MouseButtonPressedEvent&);

class UI_Button : public Widget {
public:
	UI_Button(Rect&& rect, Widget* parent, Canvas* canvas);

	virtual void Draw(const ST_REF<Renderer2D>& renderer2D) override;

	virtual void OnMouseButtonPressed(const MouseButtonPressedEvent& e) override {
		_mode = ButtonMode::PRESSED;
		Widget::OnMouseButtonPressed(e);
	}

	virtual void OnMouseButtonReleased(const MouseButtonReleasedEvent& e) override {
		_mode = ButtonMode::HOVERED;
		Widget::OnMouseButtonReleased(e);
	}

	virtual void OnMouseHovered(const MouseMovedEvent& e) override {
		_mode = ButtonMode::HOVERED;
		Widget::OnMouseHovered(e);
	}

	virtual void OnMouseLeft(const MouseMovedEvent& e) override {
		_mode = ButtonMode::NORMAL;
		Widget::OnMouseLeft(e);
	}

	Brush& GetBrush() {
		switch (_mode) {
			case ButtonMode::PRESSED: return _pressedBrush;
				break;
			case ButtonMode::HOVERED: return _hoveredBrush;
				break;
			case ButtonMode::DISABLED: return _disabledBrush;
				break;
			default: return _normalBrush;
				break;
		}
	}

	Brush _normalBrush;

	Brush _pressedBrush;

	Brush _hoveredBrush;

	Brush _disabledBrush;

protected:
	ButtonMode _mode;

	//ST_FUNC<MouseButtonPressedCallback> _mouseButtonPressedCallback;
};

}

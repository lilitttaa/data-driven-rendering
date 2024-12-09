#pragma once
#include "Brush.h"
#include "Core.h"
#include "vec3.hpp"
#include "Widget.h"

namespace ST {
class Renderer2D;

class Canvas;

struct Rect;

class Texture2D;

class UI_Image : public Widget {
public:
	UI_Image(Rect&& rect, Widget* parent, Canvas* canvas);

	virtual void Draw(const ST_REF<Renderer2D>& renderer2D) override;

	Brush _brush;
};
}

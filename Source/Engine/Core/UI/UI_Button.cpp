#include "UI_Button.h"

#include "Render/Renderer2D.h"

ST::UI_Button::UI_Button(Rect&& rect, Widget* parent, Canvas* canvas): Widget(rect, parent, canvas) {}

void ST::UI_Button::Draw(const ST_REF<Renderer2D>& renderer2D) {
	Widget::Draw(renderer2D);
	renderer2D->DrawQuad(GetGlobalRect(),GetBrush());
	renderer2D->DrawSingleLineText(GetGlobalRect()._pos,{0.1,0.1,0.1},0.4,"Button");
}

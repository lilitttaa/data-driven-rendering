#include "UI_Image.h"

#include "Render/Renderer2D.h"
#include "Render/Texture2D.h"

ST::UI_Image::UI_Image(Rect&& rect, Widget* parent, Canvas* canvas): Widget(rect, parent, canvas) {}

void ST::UI_Image::Draw(const ST_REF<Renderer2D>& renderer2D) {
	Widget::Draw(renderer2D);
	renderer2D->DrawQuad(GetGlobalRect(), _brush);
}



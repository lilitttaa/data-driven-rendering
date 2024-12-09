#include "Canvas.h"

#include "AppWindow.h"
#include "PathManager.h"
#include "UI_Button.h"
#include "UI_Image.h"
#include "Event/Event.h"
#include "Render/Renderer2D.h"

ST::Canvas::Canvas(Rect&& rect): _rect(rect) {
	auto button = ST_MAKE_REF<UI_Button>(Rect{100, 800, 100, 30}, nullptr, this);
	button->_normalBrush.SetColor(glm::vec4(0.3, 0.28, 0.32, 1));
	button->_normalBrush.SetTexture("/Resource/UI.jpg");
	button->_hoveredBrush.SetColor(glm::vec4(0.25, 0.23, 0.24, 1));
	button->_hoveredBrush.SetTexture("/Resource/UI.jpg");
	button->_pressedBrush.SetColor(glm::vec4(0.2, 0.23, 0.19, 1));
	button->_pressedBrush.SetTexture("/Resource/UI.jpg");
	//button->GetGlobalRect();
	//button->SetLocalRect(Rect(200, 700, 100, 30));

	AddChild(button);

	auto button2 = ST_MAKE_REF<UI_Button>(*button.get());
	button2->SetLocalRect(Rect(200, 700, 100, 30));
	AddChild(button2);

	auto button3 = ST_MAKE_REF<UI_Button>(Rect{500, 800, 80, 40.f}, nullptr, this);
	button3->_normalBrush.SetColor(glm::vec4(0.2, 0.2, 0.2, 1));
	button3->_normalBrush.SetTexture("/Resource/UI.jpg");
	button3->_hoveredBrush.SetColor(glm::vec4(0.2, 0.23, 0.24, 1));
	button3->_hoveredBrush.SetTexture("/Resource/UI.jpg");
	button3->_pressedBrush.SetColor(glm::vec4(0.2, 0.23, 0.19, 1));
	button3->_pressedBrush.SetTexture("/Resource/UI.jpg");
	AddChild(button3);

	auto image = ST_MAKE_REF<UI_Image>(Rect{100, 300, 100, 100}, nullptr, this);
	image->_brush.SetTexture("/Resource/NoManSky.jpg");
	AddChild(image);

	// AddChild(ST_MAKE_REF<UI_Image>(Rect{400,200,300,30},this,nullptr));
	// AddChild(ST_MAKE_REF<UI_Image>(Rect{300,300,300,30},this,nullptr));
}

void ST::Canvas::Draw(const ST_REF<Renderer2D>& renderer) {
	for (auto& widget : _children) {
		widget->Draw(renderer);
	}
	// renderer->DrawSingleChar({100,100,40,40},'c');
	// renderer->DrawSingleChar({200,200,40,40},'C');
	// renderer->DrawSingleChar({300,300,40,40},'A');
	//renderer->DrawSingleLineText({100,100},{0.1,0.1,0.1},0.4,"Text");

}

void ST::Canvas::AddChild(ST_REF<Widget> child, int idx) {
	if (idx == -1) {
		_children.push_back(child);
	}
	else {
		_children.insert(_children.begin() + idx, child);
	}
}

void ST::Canvas::OnEvent(const AppWindow& appWindow, const Event& e) {
	for (auto& child : _children) {
		double xPos, yPos;
		appWindow.GetMousePos(xPos, yPos);
		if (child->IsInWidget(xPos, yPos)) {
			switch (e.GetType()) {
				case EventType::MouseMoved: child->OnMouseHovered(*(MouseMovedEvent*)&e);
					break;
				case EventType::MouseScrolled: break;
				case EventType::MousePressed: child->OnMouseButtonPressed(*(MouseButtonPressedEvent*)&e);
					break;
				case EventType::MouseReleased: child->OnMouseButtonReleased(*(MouseButtonReleasedEvent*)&e);
					break;
				default: break;
			}
		}
		else {
			if (e.GetType() == EventType::MouseMoved && child->_cachedHovered)
				child->OnMouseLeft(*(MouseMovedEvent*)&e);
		}
	}
}

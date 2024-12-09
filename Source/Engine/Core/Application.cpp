#include "Application.h"
#include "AppWindow.h"
#include "Core.h"
#include "PathManager.h"

ST::Application::Application(): _shouldClose(false), _window(ST_MAKE_REF<AppWindow>(600, 400, false)) {}

void ST::Application::Init() {
	_window->InitWindow(this);
}

void ST::Application::Tick(float deltaTime) {
	_window->Tick(deltaTime);
}

void ST::Application::Render(float deltaTime) {
	//glClear(GL_COLOR_BUFFER_BIT);
	//_window->Clear();
	_window->Render();
	//_window->SwapBuffers();
}

void ST::Application::Destroy() {
	_window->Destroy();
}

float ST::Application::GetAPPCurrentTime() {
	return _window->GetCurrentWindowTime();
}

void ST::Application::OnEvent(const AppWindow& appWindow, const Event& e) {
	EventDisPatcher dispatcher;
	// std::bind(&OnWindowClosed,this,std::placeholders::_1);
	dispatcher.Dispatch<WindowClosedEvent>(e,ST_BIND_EVENT(ST::Application::OnWindowClosed));
	dispatcher.Dispatch<MouseButtonPressedEvent>(e, bind([](const MouseButtonPressedEvent& e)-> bool {
		return true;
	}, std::placeholders::_1));
}

bool ST::Application::OnWindowClosed(const WindowClosedEvent& e) {
	ST_LOG("OnWindowClosed\n");
	_shouldClose = true;
	return true;
}

void ST::Application::OnWindowResized(int width, int height) { }

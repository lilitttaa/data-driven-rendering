#pragma once
#include "Event/Event.h"

namespace ST {
class AppWindow;

class Application {
public:
	Application();

	virtual ~Application() {}

	virtual void Init();

	virtual void Tick(float deltaTime);

	virtual void Render(float deltaTime);

	virtual void Destroy();

	inline bool ShouldClose() {
		return _shouldClose;
	}

	virtual float GetAPPCurrentTime();

#pragma region /** Event */
	void OnEvent(const AppWindow& appWindow, const Event& e);

	bool OnWindowClosed(const WindowClosedEvent& e);

	void OnWindowResized(int width, int height);

#pragma endregion

protected:
	bool _shouldClose;

	ST_REF<AppWindow> _window;

};
}

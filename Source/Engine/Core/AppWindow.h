#pragma once

#include "Core.h"
#include "Event/EventCode.h"
#include "Render/Renderer3D.h"

/*
 * Transmit event to app
 */
namespace ST {
class DirLight;

class ImguiPanel;

class Application;

class Renderer2D;

class Canvas;

class AppWindow //:public std::enable_shared_from_this<AppWindow>
{
public:
	AppWindow(int width, int height, bool bFullScreen);

	void InitWindow(Application* app);

	void Tick(float deltaTime);

	void Render();

	void Destroy();

	void GetWindowSize(double& xSize, double& ySize) const;

	void GetMousePos(double& xPos, double& yPos) const;

	double GetCurrentWindowTime() const;

	ST_EVENT_ACTION GetKeyAction(ST_KEY_TYPE key);

	int _width;

	int _height;

private:
	GLFWwindow* _window;

	bool _bFullScreen;

	struct GLFWWindowData {
		Application* _app{};

		AppWindow* _appWindow{};

		float deltaTime = 0.0f;

		double cachedMouseXPos = 0.0;

		double cachedMouseYPos = 0.0;

	};

	ST_REF<GLFWWindowData> _userData;

	ST_REF<Canvas> _canvas;

	ST_REF<Renderer2D> _renderer2D;

	ST_REF<Renderer3D> _renderer3D;

	ST_REF<Camera> _camera;

	ST_REF<CameraController> _cameraController;

	ST_VECTOR<ST_REF<GameObject>> _gameObjects;

	ST_REF<Mesh> _skyBox;

	ST_REF<GameObject> _selectedGameObject;

	ST_REF<Mesh> _postProcessingQuad;
};
}

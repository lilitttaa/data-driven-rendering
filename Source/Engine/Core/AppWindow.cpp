#include "glad/glad.h"
#define GLFW_INCLUDE_NONE

#include "UI/ImguiPanel.h"
#include "AppWindow.h"
#include "GLFW/glfw3.h"

#include "Application.h"
#include "CameraController.h"
#include "Camera.h"
#include "GameObject.h"

#include "Log.h"
#include "MeshBuilder.h"
#include "PathManager.h"
#include "ResourceManager.h"
#include "Event/EventCode.h"
#include "Math/Transform.h"
#include "Render/Light.h"
#include "Render/Mesh.h"
#include "Render/Model.h"
#include "Render/Material.h"
#include "Render/Renderer2D.h"
#include "UI/UI_Image.h"

// #include "glad/glad.h"

// #include "glad/glad.h"

ST::AppWindow::AppWindow(int width, int height, bool bFullScreen): _width(width), _height(height),
	_bFullScreen(bFullScreen), _userData(nullptr) {}

void ST::AppWindow::InitWindow(Application* app) {
	glfwInit();
	glfwInitHint(GL_MAJOR_VERSION, 3);
	glfwInitHint(GL_MINOR_VERSION, 0);
	glfwInitHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	if (_bFullScreen) {
		GLFWmonitor* monitor    = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		_width                  = mode->width;
		_height                 = mode->height;
		_window                 = glfwCreateWindow(_width, _height, "Stellar", monitor, nullptr);
	}
	else
		_window = glfwCreateWindow(_width, _height, "Stellar", nullptr, nullptr);

	// SetIcon(window);

	if (!_window) {
		ST_LOG("Create Window Failed");
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(_window);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		ST_LOG("Load Glad Failed!");
		return;
	}
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
	glClearColor(0.3f, 0.3f, 0.3f, 1);
	//glEnable(GL_CULL_FACE);

	_canvas = ST_MAKE_REF<Canvas>(Rect{0, 0, static_cast<float>(_width), static_cast<float>(_height)});
	// ST_REF<Canvas> (new Canvas());// ST_MAKE_REF<Canvas>(});
	_renderer2D = ST_MAKE_REF<Renderer2D>(this); // ST_REF<Renderer2D>(new Renderer2D(this));
	_renderer3D = ST_MAKE_REF<Renderer3D>(this);

	_camera = ST_MAKE_REF<Camera>(1280 / 960, 90, 0.01, 1000,
		Transform{{0, 0, 10}, {0, 180, 0}, {1, 1, 1}});
	_cameraController = ST_MAKE_REF<CameraController>(_camera);
	_cameraController->SetAppWindow(this);

	ST_REF<Mesh> cubeMesh = MeshBuilder::CreateCube();
	cubeMesh->_materials  = ST::ST_VECTOR<ST_REF<Material>>{
		ST_MAKE_REF<ST::Material>("/Resource/NoManSky.jpg", "/Resource/NoManSky.jpg", "/Resource/NoManSky.jpg")
	};

	ST_REF<Mesh> planeMesh = MeshBuilder::CreatePlane();
	planeMesh->_materials  = ST::ST_VECTOR<ST_REF<Material>>{
		ST_MAKE_REF<ST::Material>("/Resource/NoManSky.jpg", "/Resource/NoManSky.jpg", "/Resource/NoManSky.jpg")
	};
	
	
	// _gameObjects.emplace_back(ST_MAKE_REF<GameObject>());
	// _gameObjects.back()->SetModel(ST_MAKE_REF<Model>(ST_VECTOR<ST_REF<Mesh>>{cubeMesh}));
	// _gameObjects.back()->_transform = Transform{};

	_gameObjects.emplace_back(ST_MAKE_REF<GameObject>());
	_gameObjects.back()->SetModel(ST_MAKE_REF<Model>(ST_VECTOR<ST_REF<Mesh>>{planeMesh}));
	_gameObjects.back()->_transform = Transform{};

	_gameObjects.emplace_back(ST_MAKE_REF<GameObject>());
	_gameObjects.back()->SetModel(ST_MAKE_REF<Model>(ST_VECTOR<ST_REF<Mesh>>{cubeMesh}));
	_gameObjects.back()->_transform = Transform{{10, 0, 0}, {}, {10, 10, 5}};

	_gameObjects.emplace_back(ST_MAKE_REF<GameObject>());
	_gameObjects.back()->SetModel(ST_MAKE_REF<Model>(ST_VECTOR<ST_REF<Mesh>>{cubeMesh}));
	_gameObjects.back()->_transform = Transform{{10, -10, 10}};

	_selectedGameObject = _gameObjects.back();
	
	_gameObjects.emplace_back(ST_MAKE_REF<GameObject>());
	_gameObjects.back()->SetModel(ST_MAKE_REF<Model>(ST_VECTOR<ST_REF<Mesh>>{cubeMesh}));
	_gameObjects.back()->_transform = Transform{{30, 20, 10}};
	

	_gameObjects.emplace_back(ST_MAKE_REF<GameObject>());
	_gameObjects.back()->SetModel(
		ResourceManager::GetResourceManager().LoadModel("/Resource/Model/nanosuit/nanosuit.obj"));
	_gameObjects.back()->_transform = Transform{{}, {0, 0, 0},};

	_skyBox=cubeMesh;
	
	_postProcessingQuad = MeshBuilder::CreateQuad();

	_userData             = ST_MAKE_REF<GLFWWindowData>();
	_userData->_app       = app;
	_userData->_appWindow = this;

#pragma region /** BindEvent */
	glfwSetWindowUserPointer(_window, _userData.get());
	glfwSetFramebufferSizeCallback(_window, [](GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
		const auto userData = static_cast<GLFWWindowData*>(glfwGetWindowUserPointer(window));
		userData->_app->OnEvent(*userData->_appWindow, WindowResizedEvent(width, height));
		userData->_appWindow->_width  = width;
		userData->_appWindow->_height = height;
	});
	glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		// const auto userData = static_cast<GLFWwindowData*>(glfwGetWindowUserPointer(window));
		// userData->app->OnEvent(key, action, mode);
		// if(key == GLFW_KEY_ESCAPE){ glfwSetWindowShouldClose(window,GL_TRUE); }
	});
	glfwSetMouseButtonCallback(_window, [](GLFWwindow* window, int button, int action, int mods) {
		const auto userData = static_cast<GLFWWindowData*>(glfwGetWindowUserPointer(window));
		switch (action) {
			case ST_PRESS: {
				MouseButtonPressedEvent pressedEvent(button);
				userData->_app->OnEvent(*userData->_appWindow, pressedEvent);
				userData->_appWindow->_canvas->OnEvent(*userData->_appWindow, pressedEvent);
				break;
			}
			case ST_RELEASE: {
				MouseButtonReleasedEvent releasedEvent(button);
				userData->_app->OnEvent(*userData->_appWindow, releasedEvent);
				userData->_appWindow->_canvas->OnEvent(*userData->_appWindow, releasedEvent);
				break;
			}
			default: break;
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	});
	glfwSetScrollCallback(_window, [](GLFWwindow* window, double xOffset, double yOffset) {
		ST_LOG("Scroll: %lf,%lf\n", xOffset, yOffset);
		const auto userData = static_cast<GLFWWindowData*>(glfwGetWindowUserPointer(window));
		userData->_appWindow->_cameraController->AddMoveSpeed(yOffset * userData->deltaTime * 10);

		// const auto userData = static_cast<GLFWwindowData*>(glfwGetWindowUserPointer(window));
		// userData->app->OnScrollChanged(xOffset,yOffset);
		// ptr->camera->SpeedUp(yoffset);
	});
	glfwSetCursorPosCallback(_window, [](GLFWwindow* window, double xPos, double yPos) {
		const auto userData = static_cast<GLFWWindowData*>(glfwGetWindowUserPointer(window));
		double xSize, ySize;
		userData->_appWindow->GetWindowSize(xSize, ySize);

		MouseMovedEvent movedEvent(xPos, ySize - yPos);
		userData->_app->OnEvent(*userData->_appWindow, movedEvent);
		userData->_appWindow->_canvas->OnEvent(*userData->_appWindow, movedEvent);

		double deltaXPos = xPos - userData->cachedMouseXPos;
		double deltaYPos = -yPos + userData->cachedMouseYPos;
		if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			userData->_appWindow->_cameraController->RotateCamera(deltaYPos * userData->deltaTime,
				deltaXPos * userData->deltaTime);
		}
		userData->cachedMouseXPos = xPos;
		userData->cachedMouseYPos = yPos;
	});
	glfwSetWindowCloseCallback(_window, [](GLFWwindow* window) {
		const auto userData = static_cast<GLFWWindowData*>(glfwGetWindowUserPointer(window));
		userData->_app->OnEvent(*userData->_appWindow, WindowClosedEvent());
		// ptr->camera->SpeedUp(yoffset);
	});
#pragma endregion

	ImguiPanel::Init(_window);

}

void ST::AppWindow::Tick(float deltaTime) {
	_userData->deltaTime = deltaTime;
	_cameraController->Tick(deltaTime);
	_camera->UpdateCameraMat();
	glfwPollEvents();

}

using namespace ST;

void ST::AppWindow::Render() {

	
	glStencilMask(0xFF); // glStencilMask(0x00) cause clearing stencil buffer bit not work
	_renderer3D->PostProcessRecordBegin();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDepthFunc(GL_LESS);
	ImguiPanel::NewFrame();

	/* Draw game objects */
	glStencilFunc(GL_ALWAYS,0,0xFF);
	glStencilMask(0x00);
	_renderer3D->BeginDraw(ResourceManager::GetResourceManager().LoadShader(
		"/Resource/OpenGLShader/BoxShader.vt.glsl",
		"/Resource/OpenGLShader/BoxShader.fg.glsl"), _camera);
	_renderer3D->SetLight();

	for (auto& gameObject : _gameObjects) {
		if(gameObject!=_selectedGameObject) {
			_renderer3D->DrawGameObject(gameObject);
		}
	}
	glDepthFunc(GL_LEQUAL);
	_renderer3D->BeginDrawSkyBox(ResourceManager::GetResourceManager().LoadShader(
		"/Resource/OpenGLShader/SkyBox.vt.glsl",
		"/Resource/OpenGLShader/SkyBox.fg.glsl"), _camera);
	_renderer3D->DrawSkyBox(_skyBox);
	glDepthFunc(GL_LESS);

	/* Draw selected game obj*/
	glStencilFunc(GL_ALWAYS,1,0xFF);
	glStencilMask(0xFF);
	_renderer3D->DrawGameObject(_selectedGameObject);
	
	glStencilFunc(GL_NOTEQUAL,1,0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	_renderer3D->BeginDraw(ResourceManager::GetResourceManager().LoadShader(
		"/Resource/OpenGLShader/PureColorShader.vt.glsl",
		"/Resource/OpenGLShader/PureColorShader.fg.glsl"), _camera);

	// _renderer3D->DrawScaledGameObjectByColor(_selectedGameObject,
	// 	{1.2, 1.2, 1.2}, {1, 1, 1, 1});
	glEnable(GL_DEPTH_TEST);
	
	// _renderer3D->BeginDraw(ResourceManager::GetResourceManager().LoadShader(
	// 	"/Resource/OpenGLShader/Lighting.vt.glsl",
	// 	"/Resource/OpenGLShader/Lighting.fg.glsl"), _camera);
	// _renderer3D->DrawLight(mesh, _camera);

	/* Draw UI */
	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	_canvas->Draw(_renderer2D);
	glEnable(GL_DEPTH_TEST);

	/* Post process */
	_renderer3D->PostProcessRecordEnd();
	// // Post Processing
	glDisable(GL_DEPTH_TEST);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	_renderer3D->BeginDraw(ResourceManager::GetResourceManager().LoadShader(
		"/Resource/OpenGLShader/PostProcessingShader.vt.glsl",
		"/Resource/OpenGLShader/PostProcessingShader.fg.glsl"), _camera);
	_renderer3D->BeginPostProcess();
	_renderer3D->DrawQuad(_postProcessingQuad);
	glEnable(GL_DEPTH_TEST);

	ImguiPanel::Render();
	glfwSwapBuffers(_window);
}

void ST::AppWindow::Destroy() {
	ImguiPanel::Close();
}

void ST::AppWindow::GetWindowSize(double& xSize, double& ySize) const {
	int iXSize, iYSize;
	glfwGetWindowSize(_window, &iXSize, &iYSize);
	xSize = static_cast<float>(iXSize);
	ySize = static_cast<float>(iYSize);
}

void ST::AppWindow::GetMousePos(double& xPos, double& yPos) const {
	glfwGetCursorPos(_window, &xPos, &yPos);
	yPos = _height - yPos;
}

double ST::AppWindow::GetCurrentWindowTime() const {
	return glfwGetTime();
}

ST_EVENT_ACTION ST::AppWindow::GetKeyAction(ST_KEY_TYPE key) {
	return glfwGetKey(_window, key);
}

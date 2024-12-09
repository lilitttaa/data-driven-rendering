#pragma once

#include "Core.h"
#include "vec3.hpp"

namespace ST {
class AppWindow;

class Camera;

class CameraController {
public:
	CameraController(ST_REF<Camera> camera): _appWindow(nullptr), _camera(camera), _speed(4), _rotateSpeed(5) {}

	void SetAppWindow(AppWindow* appWindow) {
		_appWindow = appWindow;
	}

	void SetCamera(ST_REF<Camera> camera) {
		_camera = camera;
	}

	void MoveCamera(float fVal, float rVal);

	void RotateCamera(float pitch, float yaw);

	void AddMoveSpeed(float val);
	
	void Tick(float deltaTime);

private:
	static void AddPitchAngle(glm::vec3& euler, float angle);

	static void AddYawAngle(glm::vec3& euler, float angle);
	
	AppWindow* _appWindow;

	ST_REF<Camera> _camera;

	float _speed;

	float _rotateSpeed;
};
}

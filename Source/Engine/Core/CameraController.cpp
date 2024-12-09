#include "CameraController.h"

#include "AppWindow.h"
#include "Camera.h"
#include "gtx/rotate_normalized_axis.hpp"
#include "Math/MathLibrary.h"

void ST::CameraController::MoveCamera(float fVal, float rVal) {
	const auto fVec = MathLibrary::GetForwardVec(_camera->_transform._rotator);
	MathLibrary::LogVec3(fVec);
	const auto rVec = MathLibrary::GetRightVec(_camera->_transform._rotator);
	fVal *= _speed;
	rVal *= _speed;
	const glm::vec3 offset = fVec * fVal + rVec * rVal;
	_camera->_transform._pos += offset;
}

void ST::CameraController::RotateCamera(float pitch, float yaw) {
	AddPitchAngle(_camera->_transform._rotator, pitch * _rotateSpeed);
	AddYawAngle(_camera->_transform._rotator, yaw * _rotateSpeed);

}

void ST::CameraController::AddMoveSpeed(float val) {
	_speed += val;
	if (_speed >= 100) {
		_speed = 100;
	}
	if (_speed <= 0.01f) {
		_speed = 0.01f;
	}
}

void ST::CameraController::AddPitchAngle(glm::vec3& euler, float angle) {
	euler.x -= angle;
	if (euler.x >= 89.0f)
		euler.x = 89.0f;
	if (euler.x <= -89.0f)
		euler.x = -89.0f;
}

void ST::CameraController::AddYawAngle(glm::vec3& euler, float angle) {
	euler.y -= angle;
}

void ST::CameraController::Tick(float deltaTime) {
	auto euler     = MathLibrary::QuatToEuler(_camera->_transform._rotator);
	float deltaDis = _speed * deltaTime;
	if (_appWindow->GetKeyAction(ST_KEY_W) == ST_PRESS) {
		MoveCamera(deltaDis, 0);
	}
	if (_appWindow->GetKeyAction(ST_KEY_S) == ST_PRESS) {
		MoveCamera(-deltaDis, 0);
	}
	if (_appWindow->GetKeyAction(ST_KEY_A) == ST_PRESS) {
		MoveCamera(0, -deltaDis);
	}
	if (_appWindow->GetKeyAction(ST_KEY_D) == ST_PRESS) {
		MoveCamera(0, deltaDis);
	}
}

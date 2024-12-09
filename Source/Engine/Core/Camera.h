#pragma once

#include "vec3.hpp"
#include "glm.hpp"
#include "Math/Transform.h"

namespace ST {
class Camera {
public:
	Camera(float aspect, float fov, float nearPlane, float farPlane, const Transform& trans)
		: _transform(trans), _aspect(aspect), _fov(fov), _near(nearPlane), _far(farPlane) { }

	void UpdateCameraMat();

	glm::mat4 GetViewPorjMat() const {
		return _projMat*_viewMat;
	}

	void SpeedUp(float deltaSpeed);

public:
	Transform _transform;

	float _aspect;

	float _fov;

	float _near;

	float _far;

	float _speed = 0.1f;

	float _rotateSpeed = 0.1f;

	glm::mat4 _viewMat = glm::mat4{};
	
	glm::mat4 _projMat = glm::mat4{};
};
}

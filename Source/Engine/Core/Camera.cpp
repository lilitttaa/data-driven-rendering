#include "Camera.h"

#include "ext/matrix_transform.hpp"
#include "gtx/transform.hpp"
#include "Math/MathLibrary.h"
#include "detail/type_quat.hpp"

namespace ST {
void Camera::UpdateCameraMat() {
	glm::vec3 forwardVec = MathLibrary::GetForwardVec(_transform._rotator);
	_viewMat= glm::lookAt(_transform._pos, _transform._pos + forwardVec, glm::vec3(0, 1.0f, 0));
	_projMat=glm::perspective(glm::radians(_fov), _aspect, _near, _far);

}

void Camera::SpeedUp(float deltaSpeed) {
	_speed += deltaSpeed;
	if (_speed >= 100)
		_speed = 100;
	if (_speed <= 0.1f)
		_speed = 0.1f;
}
}

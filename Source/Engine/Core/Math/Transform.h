#pragma once
#include "fwd.hpp"
#include "detail/type_quat.hpp"

namespace ST {
struct Transform {
	Transform(const glm::vec3& pos = glm::vec3(0),
		const glm::vec3& rotator   = glm::vec3(0),
		const glm::vec3& scale     = glm::vec3(1)): _pos(pos), _rotator(rotator), _scale(scale) {}
	
	glm::vec3 _pos = glm::vec3(0);

	glm::vec3 _rotator = glm::vec3(0);

	glm::vec3 _scale = glm::vec3(1);
};

}

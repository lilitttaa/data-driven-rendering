#include "MathLibrary.h"
#include "detail/type_quat.hpp"
#include "gtc/quaternion.hpp"
#include "gtx/rotate_normalized_axis.hpp"
#include "gtx/rotate_vector.hpp"

glm::vec3 ST::MathLibrary::GetForwardVec(const glm::vec3& euler) {
	auto quat = EulerToQuat(euler);
	return quat * glm::vec3(0, 0, 1);
}

glm::vec3 ST::MathLibrary::GetRightVec(const glm::vec3& euler) {
	auto quat = EulerToQuat(euler);
	return quat * glm::vec3(-1, 0, 0);
}

glm::quat ST::MathLibrary::EulerToQuat(const glm::vec3& euler) {
	return glm::quat(euler*DEG2RAD);
}

glm::vec3 ST::MathLibrary::QuatToEuler(const glm::quat& quat) {
	return glm::eulerAngles(quat)*RAD2DEG;
}




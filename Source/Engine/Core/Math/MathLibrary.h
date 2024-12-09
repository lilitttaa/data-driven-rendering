#pragma once
#include "fwd.hpp"
#include "Log.h"
#include "vec3.hpp"
#include "detail/type_quat.hpp"

namespace ST {
class MathLibrary {
public:
	static glm::vec3 GetForwardVec(const glm::vec3& euler);

	static glm::vec3 GetRightVec(const glm::vec3& euler);

	static glm::quat EulerToQuat(const glm::vec3& euler);

	static glm::vec3 QuatToEuler(const glm::quat& quat);

#define RAD2DEG (180.0f/3.1415926535f)

#define DEG2RAD (3.1415926535f/180.0f)

	// Log
	static void LogVec3(const glm::vec3& val) {
		ST_LOG("Vec: x:%f,y:%f,z:%f\n", val.x, val.y, val.z);
	}

	static void LogQuat(const glm::quat& val) {
		ST_LOG("Vec: x:%f,y:%f,z:%f,w:%f\n", val.x, val.y, val.z, val.w);
	}
};
}

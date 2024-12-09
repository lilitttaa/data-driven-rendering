#pragma once
#include "vec3.hpp"
#include "Core/Render/Shader.h"

namespace ST {
class PointLight {
public:
	PointLight(glm::vec3 pos={}, glm::vec3 Ia={}, glm::vec3 Id={}, glm::vec3 Is={},
		float constItem=.0f, float linearItem=.0f, float quadraticItem=.0f): _pos(pos),
		_ia(Ia), _id(Id), _is(Is), _const(constItem), _linear(linearItem), _quadratic(quadraticItem) {}

	// void SetUniform(Shader& shader);

	glm::vec3 _pos;

	glm::vec3 _ia;

	glm::vec3 _id;

	glm::vec3 _is;

	float _const;

	float _linear;

	float _quadratic;
};

class DirLight {
public:
	DirLight(glm::vec3 dir={}, glm::vec3 Ia={}, glm::vec3 Id={}, glm::vec3 Is={}): _dir(dir),
		_ia(Ia), _id(Id), _is(Is) {}

	// void SetUniform(Shader& shader);

	glm::vec3 _dir;

	glm::vec3 _ia;

	glm::vec3 _id;

	glm::vec3 _is;
};

}

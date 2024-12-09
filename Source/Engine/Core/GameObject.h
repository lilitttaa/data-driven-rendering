#pragma once
#include "Core.h"
#include "Math/Transform.h"

namespace ST {
class Model;

class GameObject {
public:
	void SetModel(ST_REF<Model> model) {
		_model = model;
	}

	Transform _transform;

	ST_REF<Model> _model;
};
}

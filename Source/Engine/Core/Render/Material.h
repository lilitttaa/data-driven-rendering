#pragma once
#include "Core.h"

namespace ST {
#define MATERIAL_DEFAULT_TEXTURE_PATH "/Resource/White.jpg"

class Material {
public:
	Material(): _ambientTexPath(MATERIAL_DEFAULT_TEXTURE_PATH),
		_diffuseTexPath(MATERIAL_DEFAULT_TEXTURE_PATH),
		_specularTexPath(MATERIAL_DEFAULT_TEXTURE_PATH) {}

	Material(const ST_STRING& ambientTexPath, const ST_STRING& diffuseTexPath, const ST_STRING& specularTexPath,
		float shinness = 32.f, int idx = 0): _idx(idx), _shinness(shinness),
		_ambientTexPath(ambientTexPath), _diffuseTexPath(diffuseTexPath), _specularTexPath(specularTexPath) {};

	ST_STRING& GetTexPath(int idx) {
		switch (idx) {
			case 1: return _ambientTexPath;
			case 2: return _diffuseTexPath;
			case 3: return _specularTexPath;
		}
		return _ambientTexPath;
	}

	int _idx = 0;

	float _shinness = 32.f;

	void Bind();

	void UnBind();

	ST_STRING _ambientTexPath;

	ST_STRING _diffuseTexPath;

	ST_STRING _specularTexPath;

};
}

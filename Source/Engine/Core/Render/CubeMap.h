#pragma once
#include "Core.h"

namespace ST {
class CubeMap {
public:
	CubeMap(const ST_VECTOR<ST_STRING>& imagePaths);

	void Bind() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP,_cubeMapId);
	}
	void UnBind() {
		glBindTexture(GL_TEXTURE_CUBE_MAP,0);
	}
private:
	unsigned int _cubeMapId{};
};
}

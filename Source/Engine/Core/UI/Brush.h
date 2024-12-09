#pragma once
#include "Core.h"
#include "vec4.hpp"
#include "Render/Texture2D.h"

namespace ST {
struct Brush {
	Brush(): _color(glm::vec4(1)) {}

	Brush(glm::vec4 color, const ST_STRING& texPath): _color(color),
		_texPath(texPath) { }

	void SetColor(glm::vec4 color) {
		_color = color;
	}

	void SetTexture(const ST_STRING& texPath) {
		_texPath = texPath;
	}

	glm::vec4 _color;

	ST_STRING _texPath;
};
}

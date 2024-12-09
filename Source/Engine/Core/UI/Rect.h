#pragma once
#include "vec2.hpp"

namespace ST {
struct Rect {
	Rect(): _pos{0, 0}, _size{0, 0} { }

	Rect(float x, float y, float xSize, float ySize):
		_pos({x, y}), _size({xSize, ySize}) { }

	Rect(glm::vec2&& pos, glm::vec2&& size):
		_pos(pos), _size(size) { }

	bool IsInRect(float x, float y) {
		return x > _pos.x && x < _pos.x + _size.x && y > _pos.y && y < _pos.y + _size.y;
	}

	glm::vec2 _pos;

	glm::vec2 _size;

};
}

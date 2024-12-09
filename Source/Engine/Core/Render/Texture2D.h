#pragma once

#include"Core.h"

namespace ST {
class FrameBuffer;

class Texture2D {
public:
	Texture2D(unsigned int width, unsigned int height);
	friend FrameBuffer;
	
	Texture2D(ST_STRING imagePath);

	Texture2D(unsigned int width, unsigned int height, unsigned char* buffer);

	inline ~Texture2D() {
		glDeleteTextures(1, &_textureId);
	}

	inline void Bind(int index) const {
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, _textureId);
	}

	inline void UnBind(int index) const {
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

private:
	uint32_t _textureId{};
};
}

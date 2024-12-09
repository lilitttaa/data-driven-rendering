#pragma once
#include "Font.h"
#include "Texture2D.h"
#include "vec2.hpp"

namespace ST {
struct FontCharacter {
	FontCharacter(FT_Face face, ST_FONT_CHAR c) {
		auto glyph          = face->glyph;
		unsigned int width  = glyph->bitmap.width;
		unsigned int height = glyph->bitmap.rows;
		_texture            = ST_MAKE_REF<Texture2D>(width, height, glyph->bitmap.buffer);
		_size               = {width, height};
		_bearing            = {glyph->bitmap_left, glyph->bitmap_top};
		_advance            = glyph->advance.x;
	}

	ST_REF<Texture2D> _texture;

	glm::ivec2 _size;

	glm::ivec2 _bearing;

	GLuint _advance;
};
}

#pragma once
#define FT_FREETYPE_H
#include <freetype/freetype.h>

#include "Core.h"


namespace ST {
struct FontCharacter;

using ST_FONT_CHAR = GLubyte;
using ST_FONT_UINT = FT_UInt;

class Font {
public:
	Font();
	void Init(ST_FONT_UINT width,ST_FONT_UINT height);
	ST_REF<FontCharacter> GetFontCharacter(ST_FONT_CHAR c);
protected:
	friend FontCharacter;
	ST_MAP<ST_FONT_CHAR,ST_REF<FontCharacter>> _characters;
};
}

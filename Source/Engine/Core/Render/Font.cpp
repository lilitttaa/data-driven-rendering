#include "Font.h"

#include <freetype/freetype.h>

#include "FontCharacter.h"
#include "Log.h"
#include "PathManager.h"

ST::Font::Font() {}

void ST::Font::Init(ST_FONT_UINT width, ST_FONT_UINT height) {
	/*Load font*/
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		ST_ERROR("ERROR::FREETYPE: Could not init FreeType Library\n");
	FT_Face face;
	if (FT_New_Face(ft, ST_STRING(PathManager::GetFullPath("/Resource/Fonts/arial.ttf")).c_str(), 0, &face))
		ST_ERROR("ERROR::FREETYPE: Failed to load font\n");

	FT_Set_Pixel_Sizes(face, width, height);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	for (ST_FONT_CHAR c = 0; c < 128; ++c) {
		if (FT_Load_Char(face, c,FT_LOAD_RENDER)) {
			ST_ERROR("ERROR::FREETYTPE: Failed to load Glyph\n");
		}
		if (_characters.find(c) == _characters.end()) {
			_characters.emplace(c,ST_MAKE_REF<FontCharacter>(face, c));
		}
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

ST::ST_REF<ST::FontCharacter> ST::Font::GetFontCharacter(ST_FONT_CHAR c) {
	if (_characters.find(c) != _characters.end()) {
		return _characters[c];
	}
	return nullptr;
}

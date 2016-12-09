#include "Glyph.h"
#include "GUIObject.h"

Glyph::Glyph(void)
{
}

Glyph::Glyph(FT_Face face, unsigned long code)
{
	index = FT_Get_Char_Index(face, code);
	FT_Load_Glyph(face, index, 0);
	FT_Get_Glyph(face->glyph, &ftBitmap);
	FT_Glyph_To_Bitmap(&ftBitmap, FT_RENDER_MODE_NORMAL, 0, 1);

	auto bitmap = (FT_BitmapGlyph)ftBitmap;

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, bitmap->bitmap.width, bitmap->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap->bitmap.buffer);
}

Glyph::~Glyph(void)
{
	FT_Done_Glyph(ftBitmap);
}
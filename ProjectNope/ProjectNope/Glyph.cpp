#include "Glyph.h"

#include "GUIObject.h"

#include "Mesh.h"
#include "Texture.h"
#include "MaterialList.h"

#include "GlyphAtlas.h"

extern FT_Library ftLibrary;

Glyph::Glyph(void)
{
}

Glyph::Glyph(FT_Face face, unsigned long code, GlyphAtlas * atlas)
{
	index = FT_Get_Char_Index(face, code);
	FT_Load_Glyph(face, index, 0);
	FT_Get_Glyph(face->glyph, &ftBitmap);
	FT_Glyph_To_Bitmap(&ftBitmap, FT_RENDER_MODE_NORMAL, 0, 1);

	auto bitmap = (FT_BitmapGlyph)ftBitmap;

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glBindTexture(GL_TEXTURE_2D, atlas->texture->texid);

	size_t offset_x, offset_y;
	atlas->insert(bitmap->bitmap.width, bitmap->bitmap.rows, &offset_x, &offset_y);

	glTexSubImage2D(GL_TEXTURE_2D, 0, offset_x, offset_y, bitmap->bitmap.width, bitmap->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, bitmap->bitmap.buffer);

	pos[0] = bitmap->left;
	pos[1] = -bitmap->top;
	pos[2] = bitmap->left + bitmap->bitmap.width;
	pos[3] = -bitmap->top;
	pos[4] = bitmap->left + bitmap->bitmap.width;
	pos[5] = -bitmap->top + (int)bitmap->bitmap.rows;
	pos[6] = bitmap->left + bitmap->bitmap.width;
	pos[7] = -bitmap->top + (int)bitmap->bitmap.rows;
	pos[8] = bitmap->left;
	pos[9] = -bitmap->top + (int)bitmap->bitmap.rows;
	pos[10] = bitmap->left;
	pos[11] = -bitmap->top;

	uv[0] = float(offset_x) / atlas->w;
	uv[1] = float(offset_y) / atlas->h;
	uv[2] = float(offset_x + bitmap->bitmap.width) / atlas->w;
	uv[3] = float(offset_y) / atlas->h;
	uv[4] = float(offset_x + bitmap->bitmap.width) / atlas->w;
	uv[5] = float(offset_y + bitmap->bitmap.rows) / atlas->h;
	uv[6] = float(offset_x + bitmap->bitmap.width) / atlas->w;
	uv[7] = float(offset_y + bitmap->bitmap.rows) / atlas->h;
	uv[8] = float(offset_x) / atlas->w;
	uv[9] = float(offset_y + bitmap->bitmap.rows) / atlas->h;
	uv[10] = float(offset_x) / atlas->w;
	uv[11] = float(offset_y) / atlas->h;
}

Glyph::~Glyph(void)
{
	if (ftLibrary)
		FT_Done_Glyph(ftBitmap);
}

void Glyph::insert(float * p, float * t, float offset_x, float offset_y) const
{
	for (size_t i = 0; i < 12;)
	{
		p[i] = pos[i] + offset_x;
		++i;
		p[i] = pos[i] + offset_y;
		++i;
	}

	memcpy(t, uv, 12 * sizeof(float));
}

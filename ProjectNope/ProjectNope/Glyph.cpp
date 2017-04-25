#include "Glyph.h"

#include "GUIObject.h"

#include "Mesh.h"
#include "Texture.h"
#include "MaterialList.h"

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

	texture.reset(new Texture());

	glGenTextures(1, &texture->texid);
	glBindTexture(GL_TEXTURE_2D, texture->texid);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, bitmap->bitmap.width, bitmap->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap->bitmap.buffer);

	mesh.reset(new Mesh());
	mesh->uv.push_back(Vec2(0.0f, 0.0f));
	mesh->uv.push_back(Vec2(1.0f, 0.0f));
	mesh->uv.push_back(Vec2(1.0f, 1.0f));
	mesh->uv.push_back(Vec2(0.0f, 1.0f));

	mesh->vert.push_back(Vec3(bitmap->left, bitmap->top, 0.0f));
	mesh->vert.push_back(Vec3(bitmap->left + bitmap->bitmap.width, bitmap->top, 0.0f));
	mesh->vert.push_back(Vec3(bitmap->left + bitmap->bitmap.width, (int)bitmap->top + bitmap->bitmap.rows, 0.0f));
	mesh->vert.push_back(Vec3(bitmap->left, (int)bitmap->top + bitmap->bitmap.rows, 0.0f));

	mesh->sets.push_back(FaceSet());
	mesh->sets.front().nTextures = 1;
	mesh->sets.front().vertices.push_back(Face(0, 1, 2));
	mesh->sets.front().uv_points.push_back(Face(0, 1, 2));
	mesh->sets.front().vertices.push_back(Face(2, 3, 0));
	mesh->sets.front().uv_points.push_back(Face(2, 3, 0));
}

Glyph::~Glyph(void)
{
	mesh.reset();

	if (texture != 0)
		glDeleteTextures(1, &texture->texid);

	FT_Done_Glyph(ftBitmap);
}

void Glyph::render(RenderSetup& rs)
{
	MaterialList mats;
	mats.materials.push_back(Material());
	mats.materials.back().loaded_tex.push_back(texture);
	mesh->render(rs, mats);
}
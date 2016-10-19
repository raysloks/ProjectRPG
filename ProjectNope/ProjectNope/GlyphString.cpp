#include "GlyphString.h"
#include "Glyph.h"
#include "Writing.h"
#include "GUIObject.h"

GlyphString::GlyphString(void)
{
}

GlyphString::GlyphString(const std::string& s):string(s)
{
}

GlyphString::~GlyphString(void)
{
}

void GlyphString::render(void)
{
	auto fr = Resource::get<FontResource>(font);
	if (fr!=0)
	{
		for (std::string::const_iterator c=string.cbegin();c!=string.cend();)
		{
			unsigned int code_point = Writing::getCodePoint(c, string.cend());

			auto glyph = fr->getGlyph(code_point);
			if (glyph!=0)
			{

				auto bitmap = (FT_BitmapGlyph)glyph->ftBitmap;

				glBindTexture(GL_TEXTURE_2D, glyph->texture);
				//std::cout << code_point << " : " << glyph->texture << std::endl;

				glBegin(GL_QUADS);

				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(bitmap->left, -bitmap->top);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(bitmap->bitmap.width+bitmap->left, -bitmap->top);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(bitmap->bitmap.width+bitmap->left, (int)bitmap->bitmap.rows - bitmap->top);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(bitmap->left, (int)bitmap->bitmap.rows - bitmap->top);

				glEnd();

				glTranslatef(bitmap->root.advance.x >> 16, bitmap->root.advance.y >> 16, 0.0f);

			}
		}
	}
}
#include "GlyphString.h"

#include "Glyph.h"
#include "Writing.h"
#include "RenderSetup.h"

GlyphString::GlyphString(void)
{
}

GlyphString::GlyphString(const std::string& s):string(s)
{
}

GlyphString::~GlyphString(void)
{
}

void GlyphString::render(RenderSetup& rs)
{
	auto fr = Resource::get<FontResource>(font);
	if (fr != nullptr)
	{
		for (std::string::const_iterator c=string.cbegin();c!=string.cend();)
		{
			unsigned int code_point = Writing::getCodePoint(c, string.cend());

			auto glyph = fr->getGlyph(code_point, x_size, y_size);
			if (glyph != nullptr)
			{
				glyph->render(rs);

				auto bitmap = (FT_BitmapGlyph)glyph->ftBitmap;

				rs.addTransform(Matrix4::Translation(Vec3(bitmap->root.advance.x >> 16, bitmap->root.advance.y >> 16, 0.0f)));
			}
		}
	}
}
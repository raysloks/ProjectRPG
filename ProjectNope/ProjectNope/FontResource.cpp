#include "FontResource.h"
#include "Glyph.h"

extern FT_Library ftLibrary;

FontResource::FontResource(const std::string& buf) : buffer(buf), ftFace(0)
{
}

FontResource::~FontResource(void)
{
	if (ftFace!=0)
		FT_Done_Face(ftFace);
}

FT_Face FontResource::getFace(size_t x_size, size_t y_size)
{
	if (ftFace == nullptr)
		FT_New_Memory_Face(ftLibrary, (const FT_Byte*)buffer.c_str(), buffer.size(), 0, &ftFace);
	FT_Set_Pixel_Sizes(ftFace, x_size, y_size);
	return ftFace;
}

std::shared_ptr<Glyph> FontResource::getGlyph(unsigned long code, size_t x_size, size_t y_size)
{
	auto glyph = glyphs.find(code);
	if (glyph == glyphs.end())
	{
		if (getFace(x_size, y_size) != nullptr)
		{
			auto g = new Glyph(getFace(x_size, y_size), code);
			auto glyph = std::shared_ptr<Glyph>(g);
			glyphs.insert(std::make_pair(code, glyph));
			return glyph;
		}
		return 0;
	}
	return glyph->second;
}
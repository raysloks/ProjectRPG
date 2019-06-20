#include "FontResource.h"
#include "Glyph.h"
#include "GlyphAtlas.h"

extern FT_Library ftLibrary;

FontResource::FontResource(const std::string& buf) : buffer(buf), ftFace(0)
{
}

FontResource::~FontResource(void)
{
	if (ftFace != nullptr && ftLibrary)
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
	if (!atlas)
		atlas = std::make_shared<GlyphAtlas>(2048, 2048);

	auto glyph = glyphs.find(std::make_pair(code, std::make_pair(x_size, y_size)));
	if (glyph == glyphs.end())
	{
		if (getFace(x_size, y_size) != nullptr)
		{
			auto g = new Glyph(getFace(x_size, y_size), code, atlas.get());
			auto glyph = std::shared_ptr<Glyph>(g);
			glyphs.insert(std::make_pair(std::make_pair(code, std::make_pair(x_size, y_size)), glyph));
			return glyph;
		}
		return nullptr;
	}
	return glyph->second;
}
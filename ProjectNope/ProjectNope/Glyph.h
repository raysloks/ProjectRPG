#ifndef GLYPH_H
#define GLYPH_H

#include "FontResource.h"
#include FT_GLYPH_H

class Glyph
{
public:
	Glyph(void);
	Glyph(FT_Face face, unsigned long code);
	~Glyph(void);

	unsigned int index, texture;
	FT_Glyph ftBitmap;
};

#endif
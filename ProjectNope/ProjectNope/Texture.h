#include "Resource.h"
#include "GUIObject.h"
#include "streams.h"
#include "RenderSetup.h"
#include "Vec2.h"

#ifndef Texture_H
#define Texture_H

class Texture
	: public Resource
{
public:
	Texture(void);
	Texture(instream& is);
	virtual ~Texture(void);

	unsigned short getWidth(void);
	unsigned short getHeight(void);

	virtual GLuint getGLTexID(void);

	void render(RenderSetup& rs);
	void render(RenderSetup& rs, const Vec2& size);

	bool sRGB;
	float fAnisotropicFiltering;
	static bool useGammaCorrection;
	static float fGeneralAnisotropicFiltering;

	unsigned short w, h;
	void * FinalData;

	GLuint texid;

	static void prepare();
};

#endif
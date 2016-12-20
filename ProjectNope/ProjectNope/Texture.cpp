#include "Texture.h"

float Texture::fGeneralAnisotropicFiltering = 1.0f;
bool Texture::useGammaCorrection = true;

Texture::Texture(void) : fAnisotropicFiltering(0.0f), sRGB(true)
{
}

Texture::Texture(instream& is) : fAnisotropicFiltering(0.0f), sRGB(true)
{
}

Texture::~Texture(void)
{
}

unsigned short Texture::getWidth(void)
{
	return w;
}

unsigned short Texture::getHeight(void)
{
	return h;
}

GLuint Texture::getGLTexID(void)
{
	return texid;
}
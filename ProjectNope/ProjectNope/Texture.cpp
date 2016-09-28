#include "Texture.h"

float Texture::fGeneralAnisotropicFiltering = 1.0f;
bool Texture::useGammaCorrection = true;

Texture::Texture(void) : useGeneralAnisotropicFiltering(true), sRGB(false)
{
}

Texture::Texture(instream& is) : useGeneralAnisotropicFiltering(true), sRGB(false)
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
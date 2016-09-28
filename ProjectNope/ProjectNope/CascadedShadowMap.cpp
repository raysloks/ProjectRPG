#include "CascadedShadowMap.h"

#include <iostream>

CascadedShadowMap::CascadedShadowMap(int width, int height, int splits, GLenum type, GLenum ctype)
{
	w = width;
	h = height;
	d = splits;
	t = type;
	c = ctype;

	glGenTextures(1, &depth);

	glBindTexture(GL_TEXTURE_2D_ARRAY, depth);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, t, w, h, d, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);

	if (c!=GL_NONE)
	{
		glGenTextures(1, &color);

		glBindTexture(GL_TEXTURE_2D_ARRAY, color);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, c, w, h, d, 0, GL_RGBA, GL_UNSIGNED_INT, 0);
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	glGenFramebuffers(1, &fb);
}

CascadedShadowMap::~CascadedShadowMap(void)
{
	if (fb!=0)
		glDeleteFramebuffers(1, &fb);
	if (depth!=0)
		glDeleteTextures(1, &depth);
	if (color!=0)
		glDeleteTextures(1, &color);
}

void CascadedShadowMap::resize(int width, int height, int splits)
{
	if (width!=w || height!=h || splits!=d)
	{
		w = width;
		h = height;
		d = splits;

		if (depth!=0)
		{
			glBindTexture(GL_TEXTURE_2D_ARRAY, depth);
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, t, w, h, d, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
		}
		if (color!=0)
		{
			glBindTexture(GL_TEXTURE_2D_ARRAY, color);
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, c, w, h, d, 0, GL_RGBA, GL_UNSIGNED_INT, 0);
		}
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
}

void CascadedShadowMap::bind(int layer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	if (color!=0)
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color, 0, layer);
	if (depth!=0)
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth, 0, layer);
}
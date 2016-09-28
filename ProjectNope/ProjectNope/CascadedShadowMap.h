#ifndef CASCADED_SHADOW_MAP_H
#define CASCADED_SHADWO_MAP_H

#include "GUIObject.h"

class CascadedShadowMap
{
public:
	CascadedShadowMap(int width, int height, int splits, GLenum type = GL_DEPTH_COMPONENT16, GLenum ctype = GL_NONE);
	~CascadedShadowMap(void);

	void resize(int width, int height, int splits);
	void bind(int layer);

	int w, h, d;
	GLenum t, c;

	GLuint fb, depth, color;
};

#endif
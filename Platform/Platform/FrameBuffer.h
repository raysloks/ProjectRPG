#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "GUIObject.h"

class FrameBuffer
{
public:
	FrameBuffer(unsigned short width, unsigned short height, std::vector<GLenum> gbuf, GLenum zbuf);
	~FrameBuffer(void);

	void resize(unsigned short width, unsigned short height);

	unsigned short w, h;

	std::vector<GLenum> g_type;
	GLenum z_type;

	GLuint* tex;
	GLuint depth;
	GLuint fb;
};

#endif
#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "GUIObject.h"

class Buffer;

enum FrameBufferBindType
{
	FRAME_BUFFER_DRAW,
	FRAME_BUFFER_READ,
	FRAME_BUFFER_BOTH
};

class FrameBuffer
{
public:
	FrameBuffer();
	~FrameBuffer();

	void bind(FrameBufferBindType type = FRAME_BUFFER_BOTH);
	void unbind();

	std::vector<std::shared_ptr<Buffer>> color;
	std::shared_ptr<Buffer> depth, stencil;
	
	unsigned int gl_frame_buffer;
};

#endif
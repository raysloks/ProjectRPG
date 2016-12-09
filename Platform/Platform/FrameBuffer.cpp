#include "FrameBuffer.h"

#include <iostream>

#include "Buffer.h"

FrameBuffer::FrameBuffer()
{
	glGenFramebuffers(1, &gl_frame_buffer);
}

FrameBuffer::~FrameBuffer(void)
{
	if (gl_frame_buffer!=0)
		glDeleteFramebuffers(1, &gl_frame_buffer);
}

void FrameBuffer::bind(FrameBufferBindType type)
{
	switch (type)
	{
	case FRAME_BUFFER_DRAW:
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gl_frame_buffer);
		break;
	case FRAME_BUFFER_READ:
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gl_frame_buffer);
		break;
	case FRAME_BUFFER_BOTH:
		glBindFramebuffer(GL_FRAMEBUFFER, gl_frame_buffer);
	}

	for (size_t i = 0; i < color.size(); ++i)
	{
		color[i]->refresh();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, color[i]->gl_texture_id, 0);
	}
	if (depth != nullptr)
	{
		depth->refresh();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth->gl_texture_id, 0);
	}
	if (stencil != nullptr)
	{
		stencil->refresh();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencil->gl_texture_id, 0);
	}
}

void FrameBuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

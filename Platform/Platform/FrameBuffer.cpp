#include "FrameBuffer.h"

#include <iostream>

#include "Buffer.h"

FrameBuffer::FrameBuffer()
{
	glGenFramebuffers(1, &gl_frame_buffer);
}

FrameBuffer::~FrameBuffer(void)
{
	if (gl_frame_buffer)
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

	std::vector<GLenum> draw_buffers;
	for (size_t i = 0; i < color.size(); ++i)
	{
		color[i]->refresh();
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, color[i]->gl_texture_id, 0);
		draw_buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
	}
	if (depth != nullptr)
	{
		depth->refresh();
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth->gl_texture_id, 0);
	}
	if (stencil != nullptr)
	{
		stencil->refresh();
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencil->gl_texture_id, 0);
	}

	glDrawBuffers(draw_buffers.size(), draw_buffers.data());
}

void FrameBuffer::unbind()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
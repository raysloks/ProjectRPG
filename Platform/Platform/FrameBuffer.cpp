#include "FrameBuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer(unsigned short width, unsigned short height, std::vector<GLenum> gbuf, GLenum zbuf)
{
	w = width;
	h = height;

	g_type = gbuf;
	if (g_type.size()>0)
	{
		tex = new GLuint[g_type.size()];
		glGenTextures(g_type.size(), tex);
		for (int i=0;i<g_type.size();++i)
		{
			glBindTexture(GL_TEXTURE_2D, tex[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, g_type[i], w, h, 0, GL_RGB, GL_FLOAT, NULL);
		}
	}
	else
	{
		tex = 0;
	}

	z_type = zbuf;
	if (z_type)
	{
		glGenTextures(1, &depth);
		glBindTexture(GL_TEXTURE_2D, depth);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		/*float bc[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bc);*/
		glTexImage2D(GL_TEXTURE_2D, 0, z_type, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	
	for (int i=0;i<g_type.size();++i)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, tex[i], 0);
	if (depth!=0)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

	GLenum * buffers = new GLenum[g_type.size()];
	for (int i=0;i<g_type.size();++i)
		buffers[i] = GL_COLOR_ATTACHMENT0 + i;
	glDrawBuffers(g_type.size(), buffers);
	delete [] buffers;

	/*GLint check;
	std::cout << "check: ";
	for (int i=0;i<g_type.size();++i) {
		glGetIntegerv(GL_DRAW_BUFFER0 + i, &check);
		std::cout << check << std::endl;
	}*/

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer(void)
{
	if (fb!=0)
		glDeleteFramebuffers(1, &fb);
	if (depth!=0)
		glDeleteTextures(1, &depth);
	if (tex!=0) {
		glDeleteTextures(g_type.size(), tex);
		delete [] tex;
	}
}

void FrameBuffer::resize(unsigned short width, unsigned short height)
{
	if (width!=w || height!=h)
	{
		w = width;
		h = height;
		
		for (int i=0;i<g_type.size();++i)
		{
			glBindTexture(GL_TEXTURE_2D, tex[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, g_type[i], w, h, 0, GL_RGB, GL_FLOAT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		if (depth!=0)
		{
			glBindTexture(GL_TEXTURE_2D, depth);
			glTexImage2D(GL_TEXTURE_2D, 0, z_type, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}
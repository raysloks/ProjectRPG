#include "BufferTexture.h"

#include "Buffer.h"

BufferTexture::BufferTexture(const std::shared_ptr<Buffer>& buf) : buffer(buf)
{
}

BufferTexture::~BufferTexture()
{
}

GLuint BufferTexture::getGLTexID(void)
{
	return buffer->gl_texture_id;
}
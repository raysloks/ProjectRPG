#ifndef BUFFER_TEXTURE_H
#define BUFFER_TEXTURE_H

#include "Texture.h"

class Buffer;

class BufferTexture :
	public Texture
{
public:
	BufferTexture(const std::shared_ptr<Buffer>& buffer);
	~BufferTexture();

	GLuint getGLTexID(void);

	std::shared_ptr<Buffer> buffer;
};

#endif
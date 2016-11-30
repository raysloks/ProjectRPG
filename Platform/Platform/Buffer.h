#ifndef BUFFER_H
#define BUFFER_H

enum BufferFormatType
{
	BUFFER_FLOAT,
	BUFFER_UNSIGNED_NORMALIZED,
	BUFFER_SIGNED_NORMALIZED,
	BUFFER_UNSIGNED_INTEGER,
	BUFFER_SIGNED_INTEGER,
	BUFFER_DEPTH,
	BUFFER_DEPTH_STENCIL
};

enum BufferTextureType
{
	BUFFER_1D,
	BUFFER_2D,
	BUFFER_3D,
	BUFFER_CUBE
};

class Buffer
{
public:
	Buffer();
	~Buffer();

	void resize(unsigned int w);
	void resize(unsigned int w, unsigned int h);
	void resize(unsigned int w, unsigned int h, unsigned int d);

	void setType(BufferTextureType type);

	void setFormat(BufferFormatType format, unsigned int r, unsigned int g, unsigned int b, unsigned int a);

	void refresh();

	unsigned int r, g, b, a;
	unsigned int w, h, d;
	BufferTextureType type;
	BufferFormatType format;

	bool changed;

	unsigned int gl_texture_id;
	unsigned int gl_internal_format;
	unsigned int gl_format;
	unsigned int gl_type;
};

#endif
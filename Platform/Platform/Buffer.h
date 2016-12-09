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
	BUFFER_DEPTH_STENCIL,
	BUFFER_STENCIL
};

enum BufferTextureType
{
	BUFFER_1D,
	BUFFER_2D,
	BUFFER_3D,
	BUFFER_CUBE
};

enum BufferFilterType
{
	BUFFER_NEAREST,
	BUFFER_LINEAR
};

enum BufferWrapType
{
	BUFFER_REPEAT,
	BUFFER_CLAMP
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

	void setFormat(BufferFormatType format, unsigned int r, unsigned int g = 0, unsigned int b = 0, unsigned int a = 0);

	void refresh();

	unsigned int r, g, b, a;
	unsigned int w, h, d;
	BufferTextureType type;
	BufferFormatType format;

	BufferFilterType min_filter, mag_filter;
	BufferWrapType wrap_s, wrap_t, wrap_r;
	bool use_mipmap;

	bool changed;

	unsigned int gl_texture_id;
	unsigned int gl_internal_format;
	unsigned int gl_format;
	unsigned int gl_type;
};

#endif
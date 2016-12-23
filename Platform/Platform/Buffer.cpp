#include "Buffer.h"

#include "GUIObject.h"

Buffer::Buffer()
{
	glGenTextures(1, &gl_texture_id);
	type = BUFFER_2D;
}

Buffer::~Buffer()
{
	glDeleteTextures(1, &gl_texture_id);
}

void Buffer::resize(unsigned int w)
{
	if (w != this->w)
		changed = true;
	this->w = w;
}

void Buffer::resize(unsigned int w, unsigned int h)
{
	if (w != this->w || h != this->h)
		changed = true;
	this->w = w;
	this->h = h;
}

void Buffer::resize(unsigned int w, unsigned int h, unsigned int d)
{
	if (w != this->w || h != this->h || d != this->d)
		changed = true;
	this->w = w;
	this->h = h;
	this->d = d;
}

void Buffer::setType(BufferTextureType type)
{
	if (type != this->type)
		changed = true;
	this->type = type;
}

void Buffer::setFormat(BufferFormatType format, unsigned int r, unsigned int g, unsigned int b, unsigned int a)
{
	bool new_change = false;
	if (format != this->format || r != this->r || g != this->g || b != this->b || a != this->a)
		new_change = true;
	if (new_change)
	{
		changed = true;

		this->format = format;
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;

		switch (format)
		{
		case BUFFER_FLOAT:
			gl_type = GL_FLOAT;
			if (a == 0)
				if (b == 0)
					if (g == 0)
					{
						gl_format = GL_RED;
						if (r > 16)
							gl_internal_format = GL_R32F;
						else
							gl_internal_format = GL_R16F;
					}
					else
					{
						gl_format = GL_RG;
						if (r > 16)
							gl_internal_format = GL_RG32F;
						else
							gl_internal_format = GL_RG16F;
					}
				else
				{
					gl_format = GL_RGB;
					if (r > 16)
						gl_internal_format = GL_RGB32F;
					else
						gl_internal_format = GL_RGB16F;
				}
			else
			{
				gl_format = GL_RGBA;
				if (r > 16)
					gl_internal_format = GL_RGBA32F;
				else
					gl_internal_format = GL_RGBA16F;
			}
			break;
		case BUFFER_UNSIGNED_NORMALIZED:
			gl_type = GL_UNSIGNED_INT;
			if (a == 0)
				if (b == 0)
					if (g == 0)
					{
						gl_format = GL_RED;
						if (r > 8)
							gl_internal_format = GL_R16;
						else
							gl_internal_format = GL_R8;
					}
					else
					{
						gl_format = GL_RG;
						if (r > 8)
							gl_internal_format = GL_RG16;
						else
							gl_internal_format = GL_RG8;
					}
				else
				{
					gl_format = GL_RGB;
					if (r > 8)
						gl_internal_format = GL_RGB16;
					else
						gl_internal_format = GL_RGB8;
				}
			else
			{
				gl_format = GL_RGBA;
				if (r > 8)
					gl_internal_format = GL_RGBA16;
				else
					gl_internal_format = GL_RGBA8;
			}
			break;
		case BUFFER_SIGNED_NORMALIZED:
			gl_type = GL_INT;
			if (a == 0)
				if (b == 0)
					if (g == 0)
					{
						gl_format = GL_RED;
						if (r > 8)
							gl_internal_format = GL_R16_SNORM;
						else
							gl_internal_format = GL_R8_SNORM;
					}
					else
					{
						gl_format = GL_RG;
						if (r > 8)
							gl_internal_format = GL_RG16_SNORM;
						else
							gl_internal_format = GL_RG8_SNORM;
					}
				else
				{
					gl_format = GL_RGB;
					if (r > 8)
						gl_internal_format = GL_RGB16_SNORM;
					else
						gl_internal_format = GL_RGB8_SNORM;
				}
			else
			{
				gl_format = GL_RGBA;
				if (r > 8)
					gl_internal_format = GL_RGBA16_SNORM;
				else
					gl_internal_format = GL_RGBA8_SNORM;
			}
			break;
		case BUFFER_UNSIGNED_INTEGER:
			gl_type = GL_UNSIGNED_INT;
			if (a == 0)
				if (b == 0)
					if (g == 0)
					{
						gl_format = GL_RED_INTEGER;
						if (r > 16)
							gl_internal_format = GL_R32UI;
						else
							if (r > 8)
								gl_internal_format = GL_R16UI;
							else
								gl_internal_format = GL_R8UI;
					}
					else
					{
						gl_format = GL_RG_INTEGER;
						if (r > 16)
							gl_internal_format = GL_RG32UI;
						else
							if (r > 8)
								gl_internal_format = GL_RG16UI;
							else
								gl_internal_format = GL_RG8UI;
					}
				else
				{
					gl_format = GL_RGB_INTEGER;
					if (r > 16)
						gl_internal_format = GL_RGB32UI;
					else
						if (r > 8)
							gl_internal_format = GL_RGB16UI;
						else
							gl_internal_format = GL_RGB8UI;
				}
			else
			{
				gl_format = GL_RGBA_INTEGER;
				if (r > 16)
					gl_internal_format = GL_RGBA32UI;
				else
					if (r > 8)
						gl_internal_format = GL_RGBA16UI;
					else
						gl_internal_format = GL_RGBA8UI;
			}
			break;
		case BUFFER_SIGNED_INTEGER:
			gl_type = GL_INT;
			if (a == 0)
				if (b == 0)
					if (g == 0)
					{
						gl_format = GL_RED_INTEGER;
						if (r > 16)
							gl_internal_format = GL_R32I;
						else
							if (r > 8)
								gl_internal_format = GL_R16I;
							else
								gl_internal_format = GL_R8I;
					}
					else
					{
						gl_format = GL_RG_INTEGER;
						if (r > 16)
							gl_internal_format = GL_RG32I;
						else
							if (r > 8)
								gl_internal_format = GL_RG16I;
							else
								gl_internal_format = GL_RG8I;
					}
				else
				{
					gl_format = GL_RGB_INTEGER;
					if (r > 16)
						gl_internal_format = GL_RGB32I;
					else
						if (r > 8)
							gl_internal_format = GL_RGB16I;
						else
							gl_internal_format = GL_RGB8I;
				}
			else
			{
				gl_format = GL_RGBA_INTEGER;
				if (r > 16)
					gl_internal_format = GL_RGBA32I;
				else
					if (r > 8)
						gl_internal_format = GL_RGBA16I;
					else
						gl_internal_format = GL_RGBA8I;
			}
			break;
		case BUFFER_DEPTH:
			gl_format = GL_DEPTH_COMPONENT;
			gl_type = GL_FLOAT;
			if (r > 24)
				gl_internal_format = GL_DEPTH_COMPONENT32;
			else
				if (r > 16)
					gl_internal_format = GL_DEPTH_COMPONENT24;
				else
					gl_internal_format = GL_DEPTH_COMPONENT16;
			break;
		case BUFFER_DEPTH_STENCIL:
			gl_format = GL_DEPTH_STENCIL;
			gl_type = GL_FLOAT;
			if (r > 24)
				gl_internal_format = GL_DEPTH32F_STENCIL8;
			else
				gl_internal_format = GL_DEPTH24_STENCIL8;
			break;
		case BUFFER_STENCIL:
			gl_format = GL_STENCIL_INDEX;
			gl_type = GL_UNSIGNED_INT;
			if (r > 8)
				gl_internal_format = GL_STENCIL_INDEX16;
			else if (r > 4)
				gl_internal_format = GL_STENCIL_INDEX8;
			else if (r > 1)
				gl_internal_format = GL_STENCIL_INDEX4;
			else
				gl_internal_format = GL_STENCIL_INDEX1;
			break;
		}
	}
}

void Buffer::refresh()
{
	if (changed)
	{
		changed = false;
		switch (type)
		{
		case BUFFER_1D:
			glBindTexture(GL_TEXTURE_1D, gl_texture_id);
			glTexImage1D(GL_TEXTURE_1D, 0, gl_internal_format, w, 0, gl_format, gl_type, nullptr);
			break;
		case BUFFER_2D:
			glBindTexture(GL_TEXTURE_2D, gl_texture_id);
			glTexImage2D(GL_TEXTURE_2D, 0, gl_internal_format, w, h, 0, gl_format, gl_type, nullptr);
			break;
		case BUFFER_3D:
			glBindTexture(GL_TEXTURE_3D, gl_texture_id);
			glTexImage3D(GL_TEXTURE_3D, 0, gl_internal_format, w, h, d, 0, gl_format, gl_type, nullptr);
			break;
		case BUFFER_CUBE:
			glBindTexture(GL_TEXTURE_CUBE_MAP, gl_texture_id);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, gl_internal_format, w, h, 0, gl_format, gl_type, nullptr);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, gl_internal_format, w, h, 0, gl_format, gl_type, nullptr);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, gl_internal_format, w, h, 0, gl_format, gl_type, nullptr);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, gl_internal_format, w, h, 0, gl_format, gl_type, nullptr);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, gl_internal_format, w, h, 0, gl_format, gl_type, nullptr);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, gl_internal_format, w, h, 0, gl_format, gl_type, nullptr);
			break;
		}
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
}
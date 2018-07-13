#include "Texture.h"

float Texture::fGeneralAnisotropicFiltering = 1.0f;
bool Texture::useGammaCorrection = true;

Texture::Texture(void) : fAnisotropicFiltering(0.0f), sRGB(true)
{
}

Texture::Texture(instream& is) : fAnisotropicFiltering(0.0f), sRGB(true)
{
}

Texture::~Texture(void)
{
	if (texid != 0)
		glDeleteTextures(1, &texid);
}

unsigned short Texture::getWidth(void)
{
	return w;
}

unsigned short Texture::getHeight(void)
{
	return h;
}

GLuint Texture::getGLTexID(void)
{
	return texid;
}

#include "Mesh.h"

void Texture::render(RenderSetup& rs)
{
	rs.applyMods();

	VBO vbo;
	vbo.addBuffer();
	vbo.addVertexStruct(VertexStruct("pos", 2, false, 0, 0));
	vbo.addBuffer();
	vbo.addVertexStruct(VertexStruct("uv", 2, false, 0, 0));

	float * p_data = new float[12];
	float * t_data = new float[12];

	p_data[0] = 0.0f;
	p_data[1] = 0.0f;
	p_data[2] = w;
	p_data[3] = 0.0f;
	p_data[4] = w;
	p_data[5] = h;
	p_data[6] = w;
	p_data[7] = h;
	p_data[8] = 0.0f;
	p_data[9] = h;
	p_data[10] = 0.0f;
	p_data[11] = 0.0f;

	t_data[0] = 0.0f;
	t_data[1] = 1.0f;
	t_data[2] = 1.0f;
	t_data[3] = 1.0f;
	t_data[4] = 1.0f;
	t_data[5] = 0.0f;
	t_data[6] = 1.0f;
	t_data[7] = 0.0f;
	t_data[8] = 0.0f;
	t_data[9] = 0.0f;
	t_data[10] = 0.0f;
	t_data[11] = 1.0f;

	glBindBuffer(GL_ARRAY_BUFFER, vbo.buffers[0].first);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), p_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo.buffers[1].first);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), t_data, GL_STATIC_DRAW);

	delete[] p_data;
	delete[] t_data;

	vbo.nIndices = 6;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, getGLTexID());
	vbo.draw(rs);
}
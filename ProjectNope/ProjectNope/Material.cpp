#include "Material.h"

#include "GUIObject.h"
#include "Texture.h"

Material::Material()
{
}

Material::Material(const std::string& fname)
{
	tex.push_back(fname);
}

Material::~Material()
{
}

void Material::bindTextures()
{
	if (tex.size() > loaded_tex.size())
		loaded_tex.resize(tex.size());
	for (size_t i = 0; i < tex.size(); i++)
	{
		if (loaded_tex[i] == nullptr)
			loaded_tex[i] = Resource::get<Texture>(tex[i]);
	}
	for (size_t i = 0; i < loaded_tex.size(); i++)
	{
		if (loaded_tex[i] != nullptr)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, loaded_tex[i]->getGLTexID());
		}
	}
}

void Material::setTexture(size_t index, const std::string& fname)
{
	if (index >= tex.size())
		tex.resize(index + 1);
	tex[index] = fname;
}

bool Material::operator==(const Material& rhs) const
{
	return tex == rhs.tex && loaded_tex == rhs.loaded_tex && mod == rhs.mod;
}

bool Material::operator<(const Material & rhs) const
{
	return tex < rhs.tex && loaded_tex < rhs.loaded_tex && mod < rhs.mod;
}

outstream& operator<<(outstream& os, const Material& mat)
{
	unsigned char nTex = mat.tex.size();
	os << nTex;
	for (size_t i = 0; i < nTex; ++i)
		os << mat.tex[i];
	return os;
}

instream& operator>>(instream& is, Material& mat)
{
	unsigned char nTex;
	is >> nTex;
	mat.tex.resize(nTex);
	for (size_t i = 0; i < nTex; ++i)
		is >> mat.tex[i];
	return is;
}
#ifndef MATERIAL_H
#define MATERIAL_H

#include <vector>
#include <memory>

#include "streams.h"
#include "ShaderMod.h"

class Texture;
class RenderSetup;

class Material
{
public:
	Material();
	Material(const std::string& fname);
	~Material();

	void bindTextures();

	void setTexture(size_t index, const std::string& fname);

	std::vector<std::string> tex;
	std::vector<std::shared_ptr<Texture>> loaded_tex;

	ShaderMod mod;
};

outstream& operator<<(outstream& os, const Material& mat);
instream& operator >> (instream& is, Material& mat);

#endif
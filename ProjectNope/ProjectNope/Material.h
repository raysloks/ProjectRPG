#ifndef MATERIAL_H
#define MATERIAL_H

#include <vector>
#include <memory>

#include "streams.h"
#include "ShaderMod.h"
#include "TextureDescriptor.h"

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

	std::vector<TextureDescriptor> tex;
	std::vector<std::shared_ptr<Texture>> loaded_tex;

	ShaderMod mod;

	bool operator==(const Material& rhs) const;
	bool operator<(const Material& rhs) const;
};

outstream& operator<<(outstream& os, const Material& mat);
instream& operator>>(instream& is, Material& mat);

#endif
#pragma once

#include <vector>

#include "streams.h"

class TextureDescriptor
{
public:
	TextureDescriptor();
	TextureDescriptor(const std::string& name);
	~TextureDescriptor();

	std::string name;
	std::vector<std::string> options;

	bool operator==(const TextureDescriptor& rhs) const;
	bool operator<(const TextureDescriptor& rhs) const;
};

outstream& operator<<(outstream& os, const TextureDescriptor& tex_desc);
instream& operator>>(instream& is, TextureDescriptor& tex_desc);
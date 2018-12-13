#include "TextureDescriptor.h"

TextureDescriptor::TextureDescriptor()
{
}

TextureDescriptor::TextureDescriptor(const std::string& a) : name(a)
{
}

TextureDescriptor::~TextureDescriptor()
{
}

bool TextureDescriptor::operator==(const TextureDescriptor& rhs) const
{
	return name == rhs.name;
}

bool TextureDescriptor::operator<(const TextureDescriptor& rhs) const
{
	return name < rhs.name;
}

outstream& operator<<(outstream& os, const TextureDescriptor& tex_desc)
{
	os << tex_desc.name;
	unsigned char nOpt = tex_desc.options.size();
	os << nOpt;
	for (size_t i = 0; i < nOpt; ++i)
		os << tex_desc.options[i];
	return os;
}

instream& operator>>(instream& is, TextureDescriptor& tex_desc)
{
	is >> tex_desc.name;
	unsigned char nOpt;
	is >> nOpt;
	tex_desc.options.resize(nOpt);
	for (size_t i = 0; i < nOpt; ++i)
		is >> tex_desc.options[i];
	return is;
}
#include "MaterialList.h"
#include "Mesh.h"
#include "Texture.h"

MaterialList::MaterialList(void)
{
}

MaterialList::~MaterialList(void)
{
}

outstream& operator<<(outstream& os, const MaterialList& mats)
{
	uint32_t nMats = mats.materials.size();
	os << nMats;
	for (size_t i = 0; i < nMats; ++i)
		os << mats.materials[i];
	return os;
}

instream& operator >> (instream& is, MaterialList& mats)
{
	uint32_t nMats;
	is >> nMats;
	mats.materials.resize(nMats);
	for (size_t i = 0; i < nMats; ++i)
		is >> mats.materials[i];
	return is;
}
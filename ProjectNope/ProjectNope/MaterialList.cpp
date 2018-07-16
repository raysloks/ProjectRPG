#include "MaterialList.h"
#include "Mesh.h"
#include "Texture.h"

MaterialList::MaterialList(void)
{
}

MaterialList::~MaterialList(void)
{
}

bool MaterialList::operator<(const MaterialList& rhs) const
{
	if (materials.size() < rhs.materials.size())
		return true;
	if (materials.size() > rhs.materials.size())
		return false;
	for (size_t i = 0; i < materials.size(); ++i)
	{
		if (materials[i] < rhs.materials[i])
			return true;
	}
	return false;
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
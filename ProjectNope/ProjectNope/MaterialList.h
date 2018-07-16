#ifndef MATERIAL_LIST_H
#define MATERIAL_LIST_H

#include <memory>
#include <vector>

#include "Material.h"

class Mesh;

class MaterialList
{
public:
	MaterialList(void);
	~MaterialList(void);

	std::vector<Material> materials;

	bool operator<(const MaterialList& rhs) const;
};

outstream& operator<<(outstream& os, const MaterialList& mats);
instream& operator>>(instream& is, MaterialList& mats);

#endif
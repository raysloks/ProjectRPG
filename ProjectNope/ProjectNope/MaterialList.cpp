#include "MaterialList.h"
#include "Mesh.h"
#include "Texture.h"

MaterialList::MaterialList(void)
{
}

MaterialList::~MaterialList(void)
{
}

void MaterialList::slot(const std::shared_ptr<Mesh>& mesh)
{
	for (int i=0;i<mat.size();++i) {
		auto tex = Resource::get<Texture>(mat[i]);
		mesh->slotTexture(i, tex);
	}
}
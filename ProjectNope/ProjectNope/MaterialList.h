#ifndef MATERIAL_LIST_H
#define MATERIAL_LIST_H

#include <memory>
#include <vector>

class Mesh;

class MaterialList
{
public:
	MaterialList(void);
	~MaterialList(void);

	void slot(const std::shared_ptr<Mesh>& mesh);

	std::vector<std::string> mat;
};

#endif
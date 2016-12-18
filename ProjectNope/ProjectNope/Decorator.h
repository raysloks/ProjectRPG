#ifndef DECORATOR_H
#define DECORATOR_H

#include <vector>
#include <memory>

#include "streams.h"
#include "Matrix4.h"
#include "MaterialList.h"

class RenderSetup;
class Pose;
class Mesh;

class Decorator
{
public:
	Decorator(void);
	Decorator(const std::string& mesh, const Material& material, int bone_id = -1);
	Decorator(const std::string& mesh, const MaterialList& materials, int bone_id = -1);
	~Decorator(void);

	void writeLog(outstream& os);
	void readLog(instream& is);

	void attach(const Pose& pose);
	void attach();
	void render(RenderSetup& rs);

	std::string mesh_fname;
	std::shared_ptr<Mesh> mesh, skin;
	Matrix4 local, final;
	int priority, bid;

	MaterialList materials;
};

#endif
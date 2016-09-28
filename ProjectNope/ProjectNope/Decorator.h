#ifndef DECORATOR_H
#define DECORATOR_H

#include <memory>

#include "streams.h"

#include <vector>
#include "Matrix4.h"

class RenderSetup;
class Pose;
class Mesh;

class Decorator
{
public:
	Decorator(void);
	Decorator(const std::string& mesh, const std::string& texture, int bone_id = -1);
	Decorator(const std::string& mesh, const std::vector<std::string>& textures, int bone_id = -1);
	~Decorator(void);

	void writeLog(outstream& os);
	void readLog(instream& is);

	void attach(const Pose& pose);
	void attach();
	void render(RenderSetup& rs);

	std::string _mesh;
	std::vector<std::string> _tex;
	int bid;
	std::shared_ptr<Mesh> mesh, skin;
	Matrix4 local, final;
	int priority;
};

#endif
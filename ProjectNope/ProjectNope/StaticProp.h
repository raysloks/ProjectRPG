#include "PNEntity.h"
#include "CollisionMesh.h"
#include "Matrix3.h"

#ifndef STATIC_PROP_H
#define STATIC_PROP_H

class Mesh;
class Texture;

class StaticProp : public PNEntity, public CollisionMesh
{
public:
	StaticProp(const GlobalPosition& pos, const std::string& mesh, const std::string& texture, const Matrix3& matrix = Matrix3());
	StaticProp(const GlobalPosition& pos, const std::string& mesh, const std::vector<std::string>& textures, const Matrix3& matrix = Matrix3());
	StaticProp(instream& is, bool full);
	virtual ~StaticProp(void);

	virtual void writeLog(outstream& os);
	virtual void readLog(instream& is);

	virtual void tick(float dTime);
	virtual void render(const GlobalPosition& origin);

	virtual void write_to(outstream& os, bool full) const;

	static const AutoSerialFactory<StaticProp> _factory_StaticProp;

	virtual void interpolate(Entity * pEntity, float fWeight);

	void Reconstruct(void);

	bool reconstructing, collision, camera_collision;
	std::string _mesh;
	std::shared_ptr<Mesh> mesh;
	std::vector<std::string> _tex;
	std::vector<std::shared_ptr<Texture>> tex;
	Matrix3 _mtrx;
};

#endif
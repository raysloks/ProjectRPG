#include "StaticProp.h"
#include "APL.h"
#include "Mesh.h"

#ifndef ANIMATED_COLLISION_MESH_H
#define ANIMATED_COLLISION_MESH_H

class AnimatedCollisionMesh :
	public StaticProp
{
public:
	AnimatedCollisionMesh(const GlobalPosition& pos, const Vec3& vel, const std::string& mesh, const std::string& animation, const std::string& texture, const Matrix3& matrix = Matrix3());
	AnimatedCollisionMesh(instream& is, bool full);
	AnimatedCollisionMesh(const AnimatedCollisionMesh& other);
	~AnimatedCollisionMesh(void);

	static const AutoSerialFactory<AnimatedCollisionMesh> _factory_AnimatedCollisionMeshUnit;

	virtual void writeLog(outstream& os);
	virtual void readLog(instream& is);

	virtual void tick(float dTime);
	virtual void render(const GlobalPosition& origin);

	virtual void write_to(outstream& os, bool full) const;

	virtual void interpolate(Entity * pEntity, float fWeight);

	Vec3 v;
	std::string _anim;
	Mesh *skin, *next_skin;
	std::shared_ptr<Pose> pose;
	std::shared_ptr<AnimationPoseLayer> apl;
};

#endif
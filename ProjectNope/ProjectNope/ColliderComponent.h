#ifndef COLLIDER_COMPONENT_H
#define COLLIDER_COMPONENT_H

#include "Component.h"

#include "CollisionMesh.h"
#include "GlobalPosition.h"

#include <memory>

class Collision;
class Mesh;

class ColliderComponent :
	public Component
{
public:
	ColliderComponent(bool deform = false);
	ColliderComponent(instream& is);
	~ColliderComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void tick(float dTime);

	void writeLog(outstream& os, const std::shared_ptr<ClientData>& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, const std::shared_ptr<ClientData>& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, const std::shared_ptr<ClientData>& client) const;
	void write_to(outstream& os) const;

	static AutoSerialFactory<ColliderComponent, Component> _factory;

	static void LineCheck(const GlobalPosition& sP, const GlobalPosition& eP, std::vector<std::shared_ptr<Collision>>& list);
	static void SphereCheck(const GlobalPosition& vP, float r, std::vector<std::shared_ptr<Collision>>& list);
	static void SphereCast(const GlobalPosition& sP, const GlobalPosition& eP, float r, std::vector<std::shared_ptr<Collision>>& list);
	static void DiskCast(const GlobalPosition& sP, const GlobalPosition& eP, float r, std::vector<std::shared_ptr<Collision>>& list);
	static void LowerDisk(const Vec3& lock, const Vec3& center, const Vec3& axis, const Vec3& dir, float r, std::vector<std::shared_ptr<Collision>>& list);

	thread_local static std::vector<ColliderComponent*> all;

	void update(const GlobalPosition& next, float dTime);

	GlobalPosition p, prev;
	Vec3 v;

	bool deform;

	CollisionMesh mesh;
};

#endif
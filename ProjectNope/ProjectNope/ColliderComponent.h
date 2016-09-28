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
	ColliderComponent(instream& is, bool full);
	~ColliderComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void tick(float dTime);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, ClientData& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	static const AutoSerialFactory<ColliderComponent> _factory;

	static void SphereCast(const GlobalPosition& sP, const GlobalPosition& eP, float r, std::vector<std::shared_ptr<Collision>>& list);

	static std::vector<ColliderComponent*> all;

	void update(const GlobalPosition& next_position, float dTime);

	GlobalPosition p, next;
	Vec3 v;

	bool deform;

	CollisionMesh mesh;
};

#endif
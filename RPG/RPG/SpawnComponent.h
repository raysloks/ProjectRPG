#ifndef SPAWN_COMPONENT_H
#define SPAWN_COMPONENT_H

#include "Component.h"

#include <functional>
#include <random>

#include "GlobalPosition.h"

class World;
class MobComponent;

class SpawnComponent :
	public Component
{
public:
	SpawnComponent(void);
	SpawnComponent(instream& is);
	~SpawnComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void pre_frame(float dTime);
	void post_frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, const std::shared_ptr<ClientData>& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, const std::shared_ptr<ClientData>& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, const std::shared_ptr<ClientData>& client) const;
	void write_to(outstream& os) const;

	static AutoSerialFactory<SpawnComponent, Component> _factory;

	MobComponent * spawn();
	MobComponent * spawn(const Vec3& v);
	GlobalPosition select_position(void);

	bool is_valid(const GlobalPosition& p) const;
	bool inside_area(const GlobalPosition& p) const;

	static bool in_view(const GlobalPosition& p, World * world, const std::function<bool(MobComponent*)>& pred);

	GlobalPosition aabb_min, aabb_max;

	std::default_random_engine random;
};

#endif
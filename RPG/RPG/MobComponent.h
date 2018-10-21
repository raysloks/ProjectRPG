#ifndef MOB_COMPONENT_H
#define MOB_COMPONENT_H

#include "Component.h"

#include "GlobalPosition.h"
#include "Vec3.h"
#include "Vec2.h"
#include "Quaternion.h"
#include "EntityID.h"

#include "ResourceBar.h"

#include <memory>
#include <map>
#include <functional>

#include "Mob.h"
#include "Scene.h"

class GlobalPosition;
class ActionData;
class WeaponComponent;

class MobComponent :
	public Component
{
public:
	MobComponent(void);
	MobComponent(instream& is, bool full);
	~MobComponent(void);

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

	static const AutoSerialFactory<MobComponent> _factory;

	void do_damage(size_t damage, EntityID source);
	void do_heal(size_t heal, EntityID source);

	GlobalPosition * p;

	//stats
	ResourceBar health;
	ResourceBar stamina;
	ResourceBar mana;

	Vec2 move, facing;

	Mob mob;
	Scene scene;
};

#endif
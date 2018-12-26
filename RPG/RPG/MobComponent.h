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

class PositionComponent;
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

	ASF_H(MobComponent, Component)

	void do_damage(size_t damage, EntityID source);
	void do_heal(size_t heal, EntityID source);

	PositionComponent * p;

	//stats
	ResourceBar health;
	ResourceBar stamina;
	ResourceBar mana;

	float stamina_regen;

	float speed_mod;

	size_t temp_team;

	WeaponComponent * weapon;
	size_t weapon_index;

	std::function<void(void)> on_death;
	std::function<void(float)> on_tick;

	//facing
	Vec2 facing;
	Vec3 up;

	float r;
	bool use_base_collision;

	//movement
	Vec3 v;
	Vec3 dp;
	Vec3 external_dp;
	Vec3 land_n, land_v;
	Vec3 move;
	bool run, crouch;
	float landed;

	std::map<std::string, float> input;

	// todo rework
	bool hit;
	EntityID last_hit;
};

#endif
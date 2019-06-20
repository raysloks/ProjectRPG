#pragma once

#include "Component.h"

#include "GlobalPosition.h"
#include "Vec3.h"
#include "Vec2.h"
#include "Quaternion.h"
#include "EntityID.h"

#include "ResourceBar.h"

#include "FunctionList.h"

#include "HitData.h"

#include "SyncContainer.h"

#include <memory>
#include <map>
#include <functional>

class PositionComponent;
class AnimationControlComponent;
class Aura;
class WeaponComponent;

class MobComponent :
	public Component
{
public:
	MobComponent(void);
	MobComponent(instream& is);
	~MobComponent(void);

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

	static AutoSerialFactory<MobComponent, Component> _factory;

	void do_damage(HitData& damage);
	void do_heal(size_t heal, EntityID source);

	void add_aura(Aura * aura);
	void remove_aura(Aura * aura);

	void land(float dTime);

	PositionComponent * p;
	AnimationControlComponent * acc;

	//stats
	union
	{
		struct
		{
			ResourceBar health;
			ResourceBar stamina;
			ResourceBar mana;
		};
		ResourceBar resource[3];
	};

	float stamina_regen;

	float speed_mod;

	size_t temp_team;

	WeaponComponent * weapon;
	size_t weapon_index;

	FunctionList<HitData&> on_hit_taken, on_fatal_hit_taken, on_death;

	std::vector<Aura*> auras;

	SyncContainer<uint32_t> abilities;

	//facing
	Vec2 facing;
	Vec3 up;

	float r;
	bool use_base_collision;

	//movement
	Vec3 v;
	Vec3 dp;
	Vec3 external_dp;
	Vec3 land_n, land_v, takeoff_v;
	Vec3 move, move_space;
	bool run, crouch;
	float landed;

	//actions
	bool busy, recovering, rolling;

	std::map<std::string, float> input;

	// todo rework
	bool hit;
	EntityID last_hit;

	GlobalPosition respawn;
};
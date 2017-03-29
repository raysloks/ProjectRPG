#ifndef MOB_COMPONENT_H
#define MOB_COMPONENT_H

#include "Component.h"

#include "GlobalPosition.h"
#include "Vec3.h"
#include "Vec2.h"
#include "Quaternion.h"

#include "HealthBar.h"

#include <memory>
#include <map>

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

	GlobalPosition * p;

	//stats
	HealthBar health;
	HealthBar stamina;

	WeaponComponent * weapon;
	float recoil;

	//facing
	Vec3 facing, up, move_facing;
	Vec3 cam_facing, follow;
	Quaternion cam_rot;

	float time_under_control;

	//movement
	Vec3 v;
	Vec3 dp;
	Vec3 land_n, land_v, land_g;
	Vec3 c_move;
	Vec3 move;
	bool run, landed;
	float dodge;
	bool strafe;

	std::map<std::string, float> input;

	std::shared_ptr<ActionData> action;

	bool hit;

	GlobalPosition spawn_position;
};

#endif
#ifndef WEAPON_COMPONENT_H
#define WEAPON_COMPONENT_H

#include "Component.h"

#include "EntityID.h"

class MobComponent;

class WeaponComponent :
	public Component
{
public:
	WeaponComponent(void);
	WeaponComponent(instream& is, bool full);
	~WeaponComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void pre_frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, ClientData& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	static AutoSerialFactory<WeaponComponent, Component> _factory;

	WeaponComponent * swap(size_t index);

	EntityID mob_id;

	float recoil;
};

#endif
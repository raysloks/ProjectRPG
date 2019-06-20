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
	WeaponComponent(instream& is);
	~WeaponComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void pre_frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, const std::shared_ptr<ClientData>& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, const std::shared_ptr<ClientData>& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, const std::shared_ptr<ClientData>& client) const;
	void write_to(outstream& os) const;

	static AutoSerialFactory<WeaponComponent, Component> _factory;

	WeaponComponent * swap(uint32_t index);

	void update();

	EntityID mob_id;

	uint32_t item_index;

	std::vector<EntityID> hcids;
};

#endif
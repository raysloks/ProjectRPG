#ifndef NEW_ENTITY_H
#define NEW_ENTITY_H

#include "SyncState.h"
#include "GlobalPosition.h"

#include "EntityID.h"

#include <set>

class Component;
class ClientData;
class World;

class NewEntity
{
public:
	NewEntity(void);
	NewEntity(instream& is, bool full);
	~NewEntity(void);

	SyncState ss;

	std::vector<Component*> components;
	std::vector<size_t> component_syncref;
	std::vector<uint32_t> component_sync;
	std::set<size_t> conf;

	uint32_t id;

	World * world;

	EntityID get_id(void) const;

	std::vector<Component*>::iterator addComponent(Component * pComponent, bool authority = true);
	std::vector<Component*>::iterator removeComponent(Component * pComponent);

	void pre_frame(float dTime);
	void post_frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, ClientData& client);

	void interpolate(NewEntity * pEntity, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	Component * getComponent(uint32_t type) const;
	std::vector<Component*> getComponents(uint32_t type) const;

	template <class T>
	T * getComponent(void)
	{
		for (auto i = components.begin(); i != components.end(); ++i)
			if (*i != nullptr)
				if ((*i)->_serial_id == T::_factory.id)
					return static_cast<T*>(*i);
		return nullptr;
	}

	template <class T>
	std::vector<T*> getComponents(void)
	{
		std::vector<T*> ret;
		for (auto i = components.begin(); i != components.end(); ++i)
			if (*i != 0)
				if ((*i)->_serial_id == T::_factory.id)
					ret.push_back(static_cast<T*>(*i));
		return ret;
	}
};

#endif
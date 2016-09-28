#ifndef PNENTITY_H
#define PNENTITY_H

#include "Entity.h"
#include "ScriptMemory.h"
#include "EntityLinkContainer.h"

class Chunk;
class World;

class PNEntity :
	public Entity
{
public:
	PNEntity(const GlobalPosition& pos);
	PNEntity(instream& is, bool full);
	virtual ~PNEntity(void);

	static const AutoSerialFactory<PNEntity> _factory_PNEntity;

	float range;

	Chunk * chunk;
	size_t cid;
	EntityLinkContainer links;

	std::shared_ptr<ScriptMemory> mem;

	//maybe move to somewhere else
	PNEntity * follow(size_t id);
	size_t link(PNEntity * other);
	//TODO make this more exposed(what lid does the other object get, etc)
};

#endif
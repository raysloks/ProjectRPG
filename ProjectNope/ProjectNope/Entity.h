#include "Vec2.h"
#include "Vec3.h"
#include "GlobalPosition.h"
#include "HashedString.h"
#include "Serializable.h"
#include "AutoSerialFactory.h"

#ifndef ENTITY_H
#define ENTITY_H

struct EntityID
{
	EntityID(void) : id(-1){}
	EntityID(int a, int b) : id(a), uid(b){}

	int id, uid;
};

class Entity :
	public Serializable
{
public:
	Entity(const GlobalPosition& pos);
	Entity(instream& is, bool full);
	virtual ~Entity(void);

	virtual void writeLog(outstream& os);
	virtual void readLog(instream& is);

	virtual void tick(float dTime);
	virtual void render(const GlobalPosition& origin);

	virtual void interpolate(Entity * pEntity, float fWeight);

	GlobalPosition p;
	bool instant;
	EntityID id;

	static const AutoSerialFactory<Entity> _factory_Entity;

	virtual void write_to(outstream& os, bool full) const;
};

#endif
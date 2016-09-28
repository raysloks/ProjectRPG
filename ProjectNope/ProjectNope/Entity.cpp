#include "Entity.h"
#include "GUIObject.h"

const AutoSerialFactory<Entity> Entity::_factory_Entity("Entity");

Entity::Entity(const GlobalPosition& pos) : Serializable(Entity::_factory_Entity.id), p(pos), instant(false)
{
}

Entity::Entity(instream& is, bool full) : Serializable(Entity::_factory_Entity.id), instant(false)
{
	is >> p;
}

Entity::~Entity(void)
{
}

void Entity::writeLog(outstream& os)
{
	os << p << instant;
}

void Entity::readLog(instream& is)
{
	is >> p >> instant;
}

void Entity::tick(float dTime)
{
}

void Entity::render(const GlobalPosition& origin)
{
}

void Entity::interpolate(Entity * pEntity, float fWeight)
{
	if (pEntity!=0) {
		if (pEntity->instant)
			p = pEntity->p;
		else
			p += Vec3((pEntity->p-p))*fWeight;
	}
}

void Entity::write_to(outstream& os, bool full) const
{
	os << p;
}
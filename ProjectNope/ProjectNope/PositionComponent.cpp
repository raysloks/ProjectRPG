#include "PositionComponent.h"

#include "NewEntity.h"

#include "World.h"

const AutoSerialFactory<PositionComponent> PositionComponent::_factory("PositionComponent");

PositionComponent::PositionComponent(void) : Serializable(_factory.id) {}

PositionComponent::PositionComponent(const GlobalPosition& position) : Serializable(_factory.id), p(position) {}

PositionComponent::PositionComponent(instream& is, bool full) : Serializable(_factory.id)
{
	is >> p;
}

PositionComponent::~PositionComponent(void) {}

void PositionComponent::connect(NewEntity * pEntity, bool authority)
{
	if (authority)
		sync = pEntity->ss.allocate([this] (ClientData&) {
			send = true;
		}, std::function<bool(ClientData&)>());
}

void PositionComponent::disconnect(void)
{
	if (entity->world->authority)
		entity->ss.deallocate(sync);
}

void PositionComponent::writeLog(outstream& os, ClientData& client)
{
	if (send)
		os << p;
	send = false;
}

void PositionComponent::readLog(instream& is)
{
	is >> p;
}

void PositionComponent::interpolate(Component * pComponent, float fWeight)
{
	p += Vec3((static_cast<PositionComponent*>(pComponent)->p-p))*fWeight;
}

void PositionComponent::write_to(outstream& os, ClientData& client) const
{
	os << p;
}

void PositionComponent::write_to(outstream& os) const
{
	os << p;
}

void PositionComponent::update(void)
{
	entity->ss.update(sync);
}
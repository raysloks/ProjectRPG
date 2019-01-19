#include "OrbitComponent.h"

#include "NewEntity.h"
#include "PositionComponent.h"
#include "Quaternion.h"
#include "Matrix4.h"
#include "World.h"

AutoSerialFactory<OrbitComponent, Component> OrbitComponent::_factory("OrbitComponent");

OrbitComponent::OrbitComponent(void) : Component(_factory.id), speed(1.0f)
{
}

OrbitComponent::OrbitComponent(instream& is, bool full) : Component(_factory.id)
{
}

OrbitComponent::~OrbitComponent(void)
{
}

void OrbitComponent::connect(NewEntity * pEntity, bool authority)
{
}

void OrbitComponent::disconnect(void)
{
}

void OrbitComponent::pre_frame(float dTime)
{
}

void OrbitComponent::tick(float dTime)
{
	if (entity->world->authority)
	{
		auto p = entity->getComponent<PositionComponent>();
		if (p != nullptr)
		{
			GlobalPosition off = offset * Quaternion(t * M_PI * 2.0f / period, angle);
			p->p = center + off;
			p->update();
		}
		t += dTime * speed;
	}
}

void OrbitComponent::writeLog(outstream& os, ClientData& client)
{
}

void OrbitComponent::readLog(instream& is)
{
}

void OrbitComponent::writeLog(outstream& os)
{
}

void OrbitComponent::readLog(instream& is, ClientData& client)
{
}

void OrbitComponent::interpolate(Component * pComponent, float fWeight)
{
}

void OrbitComponent::write_to(outstream& os, ClientData& client) const
{
}

void OrbitComponent::write_to(outstream& os) const
{
}
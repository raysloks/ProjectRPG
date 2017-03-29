#include "OrbitComponent.h"

#include "NewEntity.h"
#include "PositionComponent.h"
#include "Quaternion.h"
#include "Matrix4.h"

const AutoSerialFactory<OrbitComponent> OrbitComponent::_factory("OrbitComponent");

OrbitComponent::OrbitComponent(void) : Serializable(_factory.id), speed(1.0f)
{
}

OrbitComponent::OrbitComponent(instream& is, bool full) : Serializable(_factory.id)
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
	auto pc = entity->getComponent<PositionComponent>();
	if (pc != nullptr)
	{
		GlobalPosition off = offset * Quaternion(t * M_PI * 2.0f / period, angle);
		pc->p = center + off;
	}
	t += dTime * speed;
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
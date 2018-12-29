#include "CameraShakeComponent.h"

#include "NewEntity.h"
#include "World.h"

AutoSerialFactory<CameraShakeComponent, Component> CameraShakeComponent::_factory("CameraShakeComponent");

CameraShakeComponent::CameraShakeComponent(void) : Component(_factory.id)
{
	t = 0.0f;
}

CameraShakeComponent::CameraShakeComponent(instream& is, bool full) : Component(_factory.id)
{
	t = 0.0f;
}

CameraShakeComponent::~CameraShakeComponent(void)
{
}

void CameraShakeComponent::connect(NewEntity * pEntity, bool authority)
{
}

void CameraShakeComponent::disconnect(void)
{
}

void CameraShakeComponent::pre_frame(float dTime)
{
}

void CameraShakeComponent::post_frame(float dTime)
{
}

void CameraShakeComponent::tick(float dTime)
{
	t += dTime;
	if (t > 2.0f)
		entity->world->SetEntity(entity->id, nullptr);
}

void CameraShakeComponent::writeLog(outstream& os, ClientData& client)
{
}

void CameraShakeComponent::readLog(instream& is)
{
}

void CameraShakeComponent::writeLog(outstream& os)
{
}

void CameraShakeComponent::readLog(instream& is, ClientData& client)
{
}

void CameraShakeComponent::interpolate(Component * pComponent, float fWeight)
{
}

void CameraShakeComponent::write_to(outstream& os, ClientData& client) const
{
}

void CameraShakeComponent::write_to(outstream& os) const
{
}

Vec3 CameraShakeComponent::getShake(void) const
{
	float multiplicator = 1.0f - t;
	multiplicator = std::fmaxf(0.0f, multiplicator);
	float multiplicator_squared = multiplicator * multiplicator;
	float multiplicator_squared_squared = multiplicator_squared * multiplicator_squared;
	float z = t * 32.0f * multiplicator_squared_squared * multiplicator_squared_squared;
	return Vec3(0.0f, 0.0f, z);
}

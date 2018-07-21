#include "AIComponent.h"

#include "NewEntity.h"
#include "World.h"

#include "MobComponent.h"
#include "ColliderComponent.h"
#include "AnimationControlComponent.h"
#include "ProjectileComponent.h"
#include "GraphicsComponent.h"
#include "SpawnComponent.h"

#include "ShadowSpawnUnit.h"

#include "SimpleState.h"

const AutoSerialFactory<AIComponent> AIComponent::_factory("AIComponent");

AIComponent::AIComponent(void) : Serializable(_factory.id)
{
	wrapper = nullptr;
}

AIComponent::AIComponent(instream& is, bool full) : Serializable(_factory.id)
{
	wrapper = nullptr;
}

AIComponent::~AIComponent(void)
{
	if (wrapper)
	{
		delete wrapper;
	}
}

void AIComponent::connect(NewEntity * pEntity, bool authority)
{
}

void AIComponent::disconnect(void)
{
}

void AIComponent::pre_frame(float dTime)
{
}

void AIComponent::tick(float dTime)
{
	if (wrapper)
	{
		wrapper->tick(dTime);
	}
}

void AIComponent::writeLog(outstream& os, ClientData& client)
{
}

void AIComponent::readLog(instream& is)
{
}

void AIComponent::writeLog(outstream& os)
{
}

void AIComponent::readLog(instream& is, ClientData& client)
{
}

void AIComponent::interpolate(Component * pComponent, float fWeight)
{
}

void AIComponent::write_to(outstream& os, ClientData& client) const
{
}

void AIComponent::write_to(outstream& os) const
{
}
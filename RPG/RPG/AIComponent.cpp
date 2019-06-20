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

AutoSerialFactory<AIComponent, Component> AIComponent::_factory("AIComponent");

AIComponent::AIComponent(void) : Component(_factory.id)
{
	wrapper = nullptr;
}

AIComponent::AIComponent(instream& is) : Component(_factory.id)
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

void AIComponent::writeLog(outstream& os, const std::shared_ptr<ClientData>& client)
{
}

void AIComponent::readLog(instream& is)
{
}

void AIComponent::writeLog(outstream& os)
{
}

void AIComponent::readLog(instream& is, const std::shared_ptr<ClientData>& client)
{
}

void AIComponent::interpolate(Component * pComponent, float fWeight)
{
}

void AIComponent::write_to(outstream& os, const std::shared_ptr<ClientData>& client) const
{
}

void AIComponent::write_to(outstream& os) const
{
}
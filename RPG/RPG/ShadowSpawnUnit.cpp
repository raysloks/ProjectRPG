#include "ShadowSpawnUnit.h"

#include "NewEntity.h"
#include "World.h"

#include "PositionComponent.h"
#include "GraphicsComponent.h"
#include "AIComponent.h"
#include "MobComponent.h"
#include "AnimationControlComponent.h"
#include "PoseComponent.h"
#include "HitComponent.h"
#include "ProjectileComponent.h"

#include "SimpleState.h"

class ShadowSpawnAI :
	public AIWrapper
{
public:
	void tick(float dTime);

	World * world;

	PositionComponent * p;
	MobComponent * mob;
	AnimationControlComponent * acc;

	EntityID target;
};

MobComponent * ShadowSpawnUnit::spawn(const GlobalPosition& pos, World * world)
{
	return nullptr;
}

void ShadowSpawnAI::tick(float dTime)
{
}
#include "Spawner.h"
#include "GruntUnit.h"
#include "ZombieEnemyUnit.h"

#include "TestServer.h"
#include "World.h"

extern TestServer * server;

const AutoSerialFactory<Spawner> Spawner::_factory_Spawner("Spawner");

Spawner::Spawner(const GlobalPosition& pos) : PNEntity(pos), Serializable(Spawner::_factory_Spawner.id)
{
	has_spawned = false;
}

Spawner::Spawner(instream& is, bool full) : PNEntity(is, full), Serializable(Spawner::_factory_Spawner.id)
{
	has_spawned = false;
}

Spawner::~Spawner(void)
{
}

void Spawner::tick(float dTime)
{
	if (!has_spawned)
	{
		//world->AddEntity(new GruntUnit(p, Vec3()));
		//world->AddEntity(new ZombieEnemyUnit(p, Vec3(), "data/assets/unused/testbeast.gmdl", "data/assets/unused/testbeast.anim", "data/assets/terrain/textures/RockPlate.tga"));
		has_spawned = true;
	}
}
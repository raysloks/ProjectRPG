#include "TestServer.h"
#include "World.h"
extern TestServer * server;

#include "ZombieEnemyUnit.h"
#include "AnimatedCollisionMesh.h"

const AutoSerialFactory<ZombieEnemyUnit> ZombieEnemyUnit::_factory_ZombieEnemyUnit("ZombieEnemyUnit");

ZombieEnemyUnit::ZombieEnemyUnit(const GlobalPosition& pos, const Vec3& vel,
	const std::string& mesh, const std::string& animation, const std::string& texture)
	: Unit(pos, vel), Serializable(_factory_ZombieEnemyUnit.id)
{
	/*if (world->authority)
	{
		dir = 0.0f;

		acm = new AnimatedCollisionMesh(p, v, mesh, animation, texture);

		move_speed = 1.25f;

		world->AddEntity(acm);
	}*/
}

ZombieEnemyUnit::ZombieEnemyUnit(instream& is, bool full) : Unit(is, full), Serializable(_factory_ZombieEnemyUnit.id)
{
}

ZombieEnemyUnit::~ZombieEnemyUnit(void)
{
}

void ZombieEnemyUnit::tick(float dTime)
{
	if (world->authority)
	{
		dir += dTime * 0.005f;
		target = Vec2(sin(dir), -cos(dir))*3.0f;
		
		acm->v = Vec3(p + Vec3(0.0f, 0.0f, -0.01f) - acm->p)/dTime;
		acm->p = p + Vec3(0.0f, 0.0f, -0.01f);

		acm->_mtrx = Matrix3();

		acm->_mtrx.mtrx[0][0] = cos(dir);
		acm->_mtrx.mtrx[0][1] = sin(dir);
		acm->_mtrx.mtrx[1][0] = -sin(dir);
		acm->_mtrx.mtrx[1][1] = cos(dir);

		acm->_mtrx *= 30.0f;

		std::shared_ptr<SkeletalAnimation> anim = Resource::get<SkeletalAnimation>(acm->_anim);
		if (static_cast<APLList*>(acm->apl.get())->layers.size()==0 && anim)
		{
			static_cast<APLList*>(acm->apl.get())->layers.push_back(std::shared_ptr<AnimationPoseLayer>(
				new APLBlend(std::shared_ptr<Pose>(
				new Pose(anim->armature)), 0.5f, std::shared_ptr<AnimationPoseLayer>(
				new APLSpeed(5.0f, std::shared_ptr<AnimationPoseLayer>(
				new APLSource(&anim->actions.at("walk"), true)))))));
		}
	}
	Unit::tick(dTime);
}
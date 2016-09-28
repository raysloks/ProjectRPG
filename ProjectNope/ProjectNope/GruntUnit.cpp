#include "TestServer.h"
#include "TestClient.h"
#include "World.h"
extern TestServer * server;
extern TestClient * client;

#include "Decorator.h"

#include "GruntUnit.h"

const AutoSerialFactory<GruntUnit> GruntUnit::_factory_GruntUnit("GruntUnit");

#include "GruntAI.h"

#include "AttackCap.h"

GruntUnit::GruntUnit(const GlobalPosition& pos, const Vec3& vel)
	: Unit(pos, vel), Serializable(_factory_GruntUnit.id)
{
	scale = 3.0f;
	move_speed = 4.0f;
	turn_rate = 0.5f;
	turn_speed = 0.5f;
	m = 300.0f;
	ai.reset(new GruntAI(this, true));

	health = HealthBar(600);

	rAnimation = "data/assets/units/shrimp/shrimp.anim";
	Resource::load(rAnimation);

	decs.add(std::make_shared<Decorator>("data/assets/units/shrimp/shrimp.gmdl", "data/assets/units/shrimp/shrimp.tga"));

	uc.push_back(std::make_shared<AttackCap>(this, "data/scripts/grunt/attack1.txt"));
}

GruntUnit::GruntUnit(instream& is, bool full) : Unit(is, full), Serializable(_factory_GruntUnit.id)
{
	uc.push_back(std::make_shared<AttackCap>(this, "data/scripts/grunt/attack1.txt"));
}

GruntUnit::~GruntUnit(void)
{
}

#include "APL.h"

void GruntUnit::tick(float dTime)
{
	if (cs.active.find("dead")!=cs.active.end()) {
		dead = true;
		std::shared_ptr<SkeletalAnimation> anim = Resource::get<SkeletalAnimation>(rAnimation);
		if (anim!=0 && apl!=0)
		{
			cs.active.erase("dead");
			auto src = std::make_shared<APLSource>(&anim->actions["death"], false);
			src->on_end = [=] (void) -> bool {
				auto layer = std::make_shared<APLStatic>(anim->getPose(anim->actions["death"].length, anim->actions["death"]));
				apl_list->layers.push_back(layer);
				return false;
			};
			auto layer = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 1.0f, std::make_shared<APLSpeed>(60.0f, src));
			apl_list->layers.push_back(layer);
		}
	}

	if (ai!=0)
		ai->tick(dTime);
	Unit::tick(dTime);
}

void GruntUnit::_initAPL(const std::shared_ptr<SkeletalAnimation>& anim)
{
	apl_list = std::make_shared<APLList>();

	walk_spd_apl = std::make_shared<APLSpeed>(0.0f, std::make_shared<APLSource>(&anim->actions["walk_forwards"], true));
	walk_blend_apl = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.0f, walk_spd_apl);

	fall_blend_apl = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.0f, std::make_shared<APLSource>(&anim->actions["fall"], true));
	
	apl_list->layers.push_back(std::make_shared<APLStatic>(std::make_shared<Pose>(anim->armature)));
	apl_list->layers.push_back(walk_blend_apl);
	apl_list->layers.push_back(fall_blend_apl);

	apl = apl_list;
}

void GruntUnit::_tickAPL(const std::shared_ptr<SkeletalAnimation>& anim, float dTime)
{
	Vec3 v1 = v-land_v;
	Vec2 v2 = Vec2(v1.x, v1.y);
	float vl = v2.Len();
	if (vl!=0.0f)
		v2/=vl;
	float leg_move = 1.0f-time_since_landed;
	if (leg_move<0.0f)
		leg_move=0.0f;
	leg_move *= vl*10.0f;
	/*if (leg_move > 90.0f)
		leg_move = (leg_move-90.0f)*3.0f/4.0f+90.0f;*/
	if (v2.Dot(Vec2(-sin(facing.x), cos(facing.x)))<0.0f)
		leg_move = -leg_move;

	walk_spd_apl->speed = leg_move;

	walk_blend_apl->weight = std::min(1.0f, v.Len());

	fall_blend_apl->weight = std::min(1.0f, time_since_landed);
}
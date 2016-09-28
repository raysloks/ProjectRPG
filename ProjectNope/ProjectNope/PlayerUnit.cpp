#include "AudioParticle.h"

#include "TestServer.h"
#include "TestClient.h"
#include "World.h"
extern TestServer * server;
extern TestClient * client;

#include "Texture.h"
#include "Mesh.h"

#include "APL.h"

#include "Decorator.h"

#include "TargetLockCap.h"
#include "JumpCap.h"
#include "AttackCap.h"
#include "BlockCap.h"

#include "PlayerUnit.h"

const AutoSerialFactory<PlayerUnit> PlayerUnit::_factory_PlayerUnit("PlayerUnit");

extern TestServer * server;

PlayerUnit::PlayerUnit(const GlobalPosition& pos, const Vec3& vel) : Unit(pos, vel), Serializable(PlayerUnit::_factory_PlayerUnit.id)
{
	rAnimation = "data/assets/units/player/KnightGuy.anim";
	Resource::load(rAnimation);

	health = HealthBar(240);
	time_since_jump = 0.0f;

	turn_rate = 1.0f/10000.0f;
	m = 70.0f;

	scale = 0.9f;

	r = 0.5f * scale;

	combat_timer = 4.0f;

	//uc.push_back(std::make_shared<TargetLockCap>(this));
	uc.push_back(std::make_shared<JumpCap>(this, 5.0f));
	uc.push_back(std::make_shared<AttackCap>(this, "data/scripts/player/attack1.txt"));
	uc.push_back(std::make_shared<BlockCap>(this, "data/scripts/player/block.txt"));

	decs.add(std::make_shared<Decorator>("data/assets/units/player/KnightGuy.gmdl", "data/assets/units/player/KnightGuy.tga"));
	decs.add(std::make_shared<Decorator>("data/assets/decorators/mouth/mouth.gmdl", "data/assets/decorators/mouth/neutral.tga"));
	decs.add(std::make_shared<Decorator>("data/assets/decorators/eyes/left.gmdl", "data/assets/decorators/eyes/basic.tga"));
	decs.add(std::make_shared<Decorator>("data/assets/decorators/eyes/right.gmdl", "data/assets/decorators/eyes/basic.tga"));
	decs.add(std::make_shared<Decorator>("data/assets/items/armor/mail_shirt/mail_shirt.gmdl", "data/assets/items/armor/mail_shirt/mail_shirt.tga"));
	decs.add(std::make_shared<Decorator>("data/assets/items/weapons/swords/claymore.gmdl", "data/assets/items/weapons/swords/claymore.tga", 15));

	inventory.items[inventory.add(std::make_shared<Item>())]->name = "Shovel";
	inventory.items[inventory.add(std::make_shared<Item>())]->name = "Apple";
	inventory.items[inventory.add(std::make_shared<Item>())]->name = "Cheese";
	inventory.items[inventory.add(std::make_shared<Item>())]->name = "Pot of Honey";
}

PlayerUnit::PlayerUnit(instream& is, bool full) : Unit(is, full), Serializable(PlayerUnit::_factory_PlayerUnit.id)
{
	uc.push_back(std::make_shared<JumpCap>(this, 5.0f));
	uc.push_back(std::make_shared<AttackCap>(this, "data/scripts/player/attack1.txt"));
	uc.push_back(std::make_shared<BlockCap>(this, "data/scripts/player/block.txt"));
}

PlayerUnit::~PlayerUnit(void)
{
}

void PlayerUnit::_initAPL(const std::shared_ptr<SkeletalAnimation>& anim)
{
	apl_list = std::make_shared<APLList>();

	right_blend_apl = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.0f, std::make_shared<APLSource>(&anim->actions["walk_strafe_right"], true));
	left_blend_apl = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.0f, std::make_shared<APLSource>(&anim->actions["walk_strafe_left"], true));

	auto walk_src_apl = std::make_shared<APLSource>(&anim->actions["walk_forwards"], true);

	auto walk_apl = std::make_shared<APLList>();
	walk_apl->layers.push_back(walk_src_apl);
	walk_apl->layers.push_back(right_blend_apl);
	walk_apl->layers.push_back(left_blend_apl);

	if (world->authority)
	{
		auto set = std::make_shared<std::set<int>>();
		walk_src_apl->on_tick = [this, set, walk_src_apl] (float dTime) -> bool {
			int step = (walk_src_apl->time+22)/45;
			if (step>0 && set->find(step)==set->end() && time_since_landed==0.0f)
			{
				set->insert(step);
				//world->AddEntity(new AudioParticle(p, v, "data/assets/audio/punch.wav", std::min(0.1f, v.Len()*0.1f)));
			}
			return true;
		};
		walk_src_apl->on_end = [set] () -> bool {
			set->clear();
			return true;
		};
	}

	walk_spd_apl = std::make_shared<APLSpeed>(0.0f, walk_apl);
	walk_blend_apl = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.0f, walk_spd_apl);

	lower_apl = std::make_shared<APLList>();
	lower_apl->layers.push_back(std::make_shared<APLStatic>(std::make_shared<Pose>(anim->armature)));
	lower_apl->layers.push_back(walk_blend_apl);
	
	arms_walk_spd_apl = std::make_shared<APLSpeed>(0.0f, std::make_shared<APLSource>(&anim->actions["upper_body_walk_forwards_1hs"], true));
	arms_walk_blend_apl = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.0f, arms_walk_spd_apl);

	upper_apl = std::make_shared<APLList>();
	upper_apl->layers.push_back(std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 1.0f, std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.05f, std::make_shared<APLSource>(&anim->actions["idle"], true))));
	upper_apl->layers.push_back(arms_walk_blend_apl);

	fall_spd_apl = std::make_shared<APLSpeed>(12.0f, std::make_shared<APLSource>(&anim->actions["fall"], true));
	fall_blend_apl = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.0f, fall_spd_apl);

	auto fall_apl = std::make_shared<APLList>();
	fall_apl->layers.push_back(std::make_shared<APLStatic>(std::make_shared<Pose>(anim->armature)));
	fall_apl->layers.push_back(fall_blend_apl);

	tilt_blend_apl = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.0f, std::make_shared<APLStatic>(anim->getPose(0.0f, "tilt_forwards")));

	auto tilt_apl = std::make_shared<APLList>();
	tilt_apl->layers.push_back(std::make_shared<APLStatic>(std::make_shared<Pose>(anim->armature)));
	tilt_apl->layers.push_back(tilt_blend_apl);

	apl_list->layers.push_back(std::make_shared<APLStatic>(std::make_shared<Pose>(anim->armature)));
	apl_list->layers.push_back(std::make_shared<APLAdd>(std::make_shared<Pose>(anim->armature), lower_apl));
	apl_list->layers.push_back(std::make_shared<APLAdd>(std::make_shared<Pose>(anim->armature), upper_apl));
	apl_list->layers.push_back(std::make_shared<APLAdd>(std::make_shared<Pose>(anim->armature), fall_apl));
	apl_list->layers.push_back(std::make_shared<APLAdd>(std::make_shared<Pose>(anim->armature), tilt_apl));

	apl = apl_list;
}

void PlayerUnit::_tickAPL(const std::shared_ptr<SkeletalAnimation>& anim, float dTime)
{
	Vec3 v1 = v-land_v;
	Vec2 v2 = Vec2(v1.x, v1.y);
	float vl = v2.Len();
	if (vl!=0.0f)
		v2/=vl;

	float walk_blend_target_weight = std::min(1.0f, v1.Len()*200.0f/scale);

	walk_blend_apl->weight -= walk_blend_target_weight;
	float walk_dif = 10.0f*dTime;
	if (abs(walk_blend_apl->weight)<=walk_dif) {
		walk_blend_apl->weight = 0.0f;
	} else {
		if (walk_blend_apl->weight<0.0f)
			walk_blend_apl->weight += walk_dif;
		else
			walk_blend_apl->weight -= walk_dif;
	}
	walk_blend_apl->weight += walk_blend_target_weight;
	arms_walk_blend_apl->weight = walk_blend_apl->weight;

	float leg_move = std::max(0.0f, 1.0f-time_since_landed);
	v1.z *= leg_move;
	leg_move *= vl/**v2.Dot(Vec2(-sin(facing.x), cos(facing.x)))*//*/getSize()*/*60.0f;
	leg_move /= scale;
	if (leg_move > 90.0f)
		leg_move = (leg_move-90.0f)*3.0f/4.0f+90.0f;
	if (leg_move > 120.0f)
		leg_move = (leg_move-120.0f)/2.0f+120.0f;

	float lr = v2.Dot(Vec2(cos(facing.x), sin(facing.x)));
	if (v2.Dot(Vec2(-sin(facing.x), cos(facing.x)))<0.0f) {
		leg_move = -leg_move;
		lr = -lr;
	}

	float prev_lr = sin(right_blend_apl->weight*M_PI/2.0f)-sin(left_blend_apl->weight*M_PI/2.0f);

	prev_lr -= lr;
	prev_lr *= exp(log(0.0001f)*dTime);
	prev_lr += lr;

	right_blend_apl->weight = std::max<float>(0.0f, asin(std::min(prev_lr, 1.0f))/M_PI*2.0f);
	left_blend_apl->weight = std::max<float>(0.0f, asin(std::min(-prev_lr, 1.0f))/M_PI*2.0f);

	walk_spd_apl->speed = leg_move;
	arms_walk_spd_apl->speed = leg_move;

	float fall_blend_target_weight = std::min(1.0f, std::max(0.0f, time_since_landed-0.1f));
	fall_blend_apl->weight -= fall_blend_target_weight;
	float fall_dif = 30.0f*dTime;
	if (abs(fall_blend_apl->weight)<=fall_dif) {
		fall_blend_apl->weight = 0.0f;
	} else {
		if (fall_blend_apl->weight<0.0f)
			fall_blend_apl->weight += fall_dif;
		else
			fall_blend_apl->weight -= fall_dif;
	}
	fall_blend_apl->weight += fall_blend_target_weight;

	Vec3 lean = v-last_v+Vec3(0.0f, 0.0f, 8.0f);
	if (lean.z<0.0f)
		lean.z=0.0f;
	lean /= lean.Len();
	Vec3 axis = Vec3(0.0f, 0.0f, 1.0f).Cross(lean);
	
	float tilt_blend_weight = lean.Dot(Vec2(-sin(facing.x), cos(facing.x)));

	if (tilt_blend_weight<0.0f || combat_timer<4.0f)
		tilt_blend_weight /= 8.0f;
	
	tilt_blend_apl->weight -= tilt_blend_weight;
	tilt_blend_apl->weight *= exp(log(0.0005f)*dTime);
	tilt_blend_apl->weight += tilt_blend_weight;
}

void PlayerUnit::writeLog(outstream& os)
{
	Unit::writeLog(os);
	os << last_v;
}

void PlayerUnit::readLog(instream& is)
{
	Unit::readLog(is);
	is >> last_v;
}

void PlayerUnit::equip(const std::shared_ptr<Item>& item)
{
}

void PlayerUnit::unequip(const std::shared_ptr<Item>& item)
{
	decs.remove(item->dec);
}

#include "Profiler.h"

void PlayerUnit::tick(float dTime)
{
	move_speed = 5.0f;
	if (cs.input["shift"]>0.5f)
		move_speed = 9.0f;
	
	if (world->authority)
	{
		std::string prev = decs.items[1]->_tex[0];
		if (cs.input["emote"]>0.5f)
			decs.items[1]->_tex[0]="data/assets/decorators/mouth/test.tga";
		else
			decs.items[1]->_tex[0]="data/assets/decorators/mouth/neutral.tga";
		if (prev!=decs.items[1]->_tex[0])
			decs.update(1);
	}

	if (cs.active.find("dead")!=cs.active.end()) {
		if (world->authority)
		{
			decs.items[2]->_tex[0]="data/assets/decorators/eyes/cross.tga";
			decs.items[3]->_tex[0]="data/assets/decorators/eyes/cross.tga";
			decs.update(2);
			decs.update(3);
		}
		dead = true;
		std::shared_ptr<SkeletalAnimation> anim = Resource::get<SkeletalAnimation>(rAnimation);
		if (anim!=0 && apl!=0)
		{
			cs.active.erase("dead");
			auto src = std::make_shared<APLSource>(&anim->actions["death"], false);
			if (world->authority)
			{
				src->on_end = [this] () -> bool {
					//world->SetEntity(this->id, 0);
					return false;
				};
			}
			auto layer = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 1.0f, std::make_shared<APLSpeed>(60.0f, src));
			apl_list->layers.push_back(layer);
		}
	}
	if (cs.active.find("hit")!=cs.active.end()) {
		cs.active.erase("hit");
		if (!dead)
		{
			if (world->authority)
			{
				//world->AddEntity(new AudioParticle(p, v, "data/assets/audio/ouch.wav"));
			}
			if (client!=0)
			{
				std::shared_ptr<SkeletalAnimation> anim = Resource::get<SkeletalAnimation>(rAnimation);
				if (anim!=0 && apl!=0)
				{
					auto src = std::make_shared<APLSource>(&anim->actions["hit1"], false);
					auto layer = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 1.0f, std::make_shared<APLSpeed>(60.0f, src));
					apl_list->layers.push_back(layer);
				}
			}
		}
	}

	if (world->authority)
	{
		last_v -= v;
		last_v *= exp(log(0.033f)*dTime);
		last_v += v;

		time_since_jump += dTime;

		if (combat_timer>=4.0f) {
			if (cs.input.find("dir_x")!=cs.input.end())
				cs.input.erase("dir_x");
		} else {
			combat_timer += dTime;
		}

		//move_speed = 6.0f+3.0f*std::min(0.0f, target.Dot(Vec2(-sin(facing.x), cos(facing.x))));
		//move_speed = 4.5f+1.5f*std::max(0.0f, target.Dot(Vec2(-sin(facing.x), cos(facing.x))));
	}

	Unit::tick(dTime);

	Profiler::add("player-speed", v.Len());
	if (time_since_landed==0.0f)
		Profiler::add("landed", 9001.0f);
}

#include "Polynome.h"

void PlayerUnit::render(const GlobalPosition& origin)
{
	Unit::render(origin);
	
	Vec3 v0 = v;
	Vec3 g = Vec3(0.0f, 0.0f, -9.8f);
	v0 -= land_v;
	if (time_since_landed<=0.2f && time_since_jump>0.2f)
	{
		if (v0.z < 0.0f)
			v0.z = 0.0f;
		v0.z += 5.0f*std::min(1.0f, 1.5f-std::max(0.5f, -cs.input["dir_y"]*1.5f*2.0f/(float)M_PI));
	}

	glPushMatrix();

	Vec3 pos = p-origin;
	glTranslatef(pos.x, pos.y, pos.z);
	glTranslatef(0.0f, 0.0f, -r);

	Polynome poly;
	poly.k.push_back(Vec3());
	poly.k.push_back(v0);
	poly.k.push_back(g*0.5f);

	poly.start = 0.0f;
	poly.end = 5.0f;

	//poly.render();

	glPopMatrix();
}

void PlayerUnit::interpolate(Entity * pEntity, float fWeight)
{
	Unit::interpolate(pEntity, fWeight);
	PlayerUnit * pUnit = dynamic_cast<PlayerUnit*>(pEntity);
	if (pUnit!=0) {
		last_v *= 1.0f-fWeight;
		last_v += pUnit->last_v*fWeight;
		/*if (pUnit->time_attack<time_attack)
			time_attack = 0.0f;
		time_attack *= 1.0f-fWeight;
		time_attack += pUnit->time_attack*fWeight;*/
	}
}

void PlayerUnit::write_to(outstream& os, bool full) const
{
	Unit::write_to(os, full);
}
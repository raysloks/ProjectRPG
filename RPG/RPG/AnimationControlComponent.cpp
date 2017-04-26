#include "AnimationControlComponent.h"

#include "NewEntity.h"

#include "World.h"

#include "Resource.h"

#include "StringResource.h"

#include "SkeletalAnimation.h"
#include "AnimationPoseLayer.h"
#include "APL.h"

#include "MobComponent.h"
#include "HitComponent.h"
#include "AudioComponent.h"
#include "PositionComponent.h"
#include "PoseComponent.h"
#include "GraphicsComponent.h"

#include "BlendUtility.h"

#include "Script.h"

#include "ActionData.h"

#include "Profiler.h"

const AutoSerialFactory<AnimationControlComponent> AnimationControlComponent::_factory("AnimationControlComponent");

AnimationControlComponent::AnimationControlComponent(void) : Serializable(_factory.id)
{
}

AnimationControlComponent::AnimationControlComponent(instream& is, bool full) : Serializable(_factory.id)
{
	synchro.readLog(is);
	is >> cs;
}

AnimationControlComponent::~AnimationControlComponent(void)
{
}

void AnimationControlComponent::connect(NewEntity * pEntity, bool authority)
{
}

void AnimationControlComponent::disconnect(void)
{
}

void AnimationControlComponent::initAPL(void)
{
	apl_list = std::make_shared<APLList>();

	auto walk_right_src_apl = std::make_shared<APLSource>(&anim->actions["walk_strafe_right"], true);
	auto walk_left_src_apl = std::make_shared<APLSource>(&anim->actions["walk_strafe_left"], true);

	right_blend_apl = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.0f, walk_right_src_apl);
	left_blend_apl = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.0f, walk_left_src_apl);

	auto back_src_apl = std::make_shared<APLSource>(&anim->actions["walk_forwards"], true);
	auto back_right_src_apl = std::make_shared<APLSource>(&anim->actions["walk_strafe_left"], true);
	auto back_left_src_apl = std::make_shared<APLSource>(&anim->actions["walk_strafe_right"], true);

	back_right_blend_apl = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.0f, back_right_src_apl);
	back_left_blend_apl = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.0f, back_left_src_apl);

	auto back_apl = std::make_shared<APLList>();
	back_apl->layers.push_back(back_src_apl);
	back_apl->layers.push_back(back_right_blend_apl);
	back_apl->layers.push_back(back_left_blend_apl);

	back_blend_apl = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.0f, std::make_shared<APLSpeed>(-1.0f, back_apl));

	run_blend_apl = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.0f, std::make_shared<APLSource>(&anim->actions["run_forwards"], true));

	auto walk_src_apl = std::make_shared<APLSource>(&anim->actions["walk_forwards"], true);

	auto walk_apl = std::make_shared<APLList>();
	walk_apl->layers.push_back(walk_src_apl);
	walk_apl->layers.push_back(run_blend_apl);
	walk_apl->layers.push_back(right_blend_apl);
	walk_apl->layers.push_back(left_blend_apl);
	walk_apl->layers.push_back(back_blend_apl);

	if (entity->world->authority)
	{
		auto set = std::make_shared<std::set<int>>();
		walk_src_apl->on_tick = [this, set, walk_src_apl](float dTime) -> bool {
			int step = (walk_src_apl->time+22)/45;
			if (step>0 && set->find(step)==set->end() && mob->landed)
			{
				set->insert(step);
				//world->AddEntity(new AudioParticle(p, v, "data/assets/audio/punch.wav", std::fminf(0.1f, v.Len()*0.1f)));
			}
			return true;
		};
		walk_src_apl->on_end = [set]() -> bool {
			set->clear();
			return true;
		};
	}

	walk_spd_apl = std::make_shared<APLSpeed>(0.0f, walk_apl);
	walk_blend_apl = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.0f, walk_spd_apl);

	lower_apl = std::make_shared<APLList>();
	lower_apl->layers.push_back(std::make_shared<APLStatic>(std::make_shared<Pose>(anim->armature)));
	lower_apl->layers.push_back(walk_blend_apl);

	auto arms_walk_src_apl = std::make_shared<APLSource>(&anim->actions["upper_body_walk_forwards"], true);
	arms_walk_spd_apl = std::make_shared<APLSpeed>(0.0f, arms_walk_src_apl);
	arms_walk_blend_apl = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.0f, arms_walk_spd_apl);

	auto idle_src_apl = std::make_shared<APLSource>(&anim->actions["idle"], true);
	idle_blend_apl = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 1.0f, std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 0.05f, idle_src_apl));

	upper_apl = std::make_shared<APLList>();
	upper_apl->layers.push_back(std::make_shared<APLStatic>(std::make_shared<Pose>(anim->armature)));
	upper_apl->layers.push_back(idle_blend_apl);
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

	turn_left_src_apl = std::make_shared<APLSource>(&anim->actions["turn_left"], true);
	turn_right_src_apl = std::make_shared<APLSource>(&anim->actions["turn_right"], true);

	auto turn_apl = std::make_shared<APLList>();
	turn_apl->layers.push_back(std::make_shared<APLStatic>(std::make_shared<Pose>(anim->armature)));
	turn_apl->layers.push_back(std::make_shared<APLAdd>(std::make_shared<Pose>(anim->armature), turn_left_src_apl));
	turn_apl->layers.push_back(std::make_shared<APLAdd>(std::make_shared<Pose>(anim->armature), turn_right_src_apl));

	action_apl = std::make_shared<APLList>();
	action_apl->layers.push_back(std::make_shared<APLStatic>(std::make_shared<Pose>(anim->armature)));

	apl_list->layers.push_back(std::make_shared<APLStatic>(std::make_shared<Pose>(anim->armature)));
	apl_list->layers.push_back(std::make_shared<APLAdd>(std::make_shared<Pose>(anim->armature), action_apl));
	apl_list->layers.push_back(std::make_shared<APLAdd>(std::make_shared<Pose>(anim->armature), std::make_shared<APLSpeed>(0.0f, turn_apl)));
	apl_list->layers.push_back(std::make_shared<APLAdd>(std::make_shared<Pose>(anim->armature), lower_apl));
	apl_list->layers.push_back(std::make_shared<APLAdd>(std::make_shared<Pose>(anim->armature), upper_apl));
	apl_list->layers.push_back(std::make_shared<APLAdd>(std::make_shared<Pose>(anim->armature), fall_apl));
	apl_list->layers.push_back(std::make_shared<APLAdd>(std::make_shared<Pose>(anim->armature), tilt_apl));

	apl = apl_list;

	synchro.var.push_back(std::pair<float, float*>(walk_right_src_apl->action->length, &walk_right_src_apl->time));
	synchro.var.push_back(std::pair<float, float*>(walk_left_src_apl->action->length, &walk_left_src_apl->time));
	synchro.var.push_back(std::pair<float, float*>(back_src_apl->action->length, &back_src_apl->time));
	synchro.var.push_back(std::pair<float, float*>(back_right_src_apl->action->length, &back_right_src_apl->time));
	synchro.var.push_back(std::pair<float, float*>(back_left_src_apl->action->length, &back_left_src_apl->time));
	synchro.var.push_back(std::pair<float, float*>(walk_src_apl->action->length, &walk_src_apl->time));
	synchro.var.push_back(std::pair<float, float*>(arms_walk_src_apl->action->length, &arms_walk_src_apl->time));
	synchro.var.push_back(std::pair<float, float*>(idle_src_apl->action->length, &idle_src_apl->time));
}

#include "Profiler.h"

void AnimationControlComponent::tickAPL(float dTime)
{
	HitComponent * hit = entity->getComponent<HitComponent>();

	if (hit != 0)
		hit->mtrx = pose->bones[15].total_transform;

	if (mob->input.find("attack") != mob->input.end() && mob->action == 0 && entity->world->authority)
	{
		if (mob->stamina.current > 0.0f)
		{
			if (start_action("data/scripts/attack1.txt"))
			{
				mob->input.erase("attack");
				cs.activate("data/scripts/attack1.txt");

				mob->stamina.current -= 4;
			}
		}
	}

	if (!entity->world->authority)
	{
		if (cs.active.size() != 0)
		{
			if (start_action(cs.active.begin()->first))
				cs.active.erase(cs.active.begin());
		}
	}

	if (mob->hit)
	{
		mob->health.current -= 1;

		float blend_in = 1.0f;
		float blend_out = 8.0f;

		auto src = std::make_shared<APLSource>(&anim->actions["hit1"], false);
		auto layer = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 1.0f, std::make_shared<APLSpeed>(20.0f, src));

		std::weak_ptr<APLSource> w_src(src);
		std::weak_ptr<APLBlend> w_layer(layer);

		src->on_tick = [=](float dTime) -> bool {
			auto src = w_src.lock();
			auto layer = w_layer.lock();
			layer->weight = -cos(std::fminf(1.0f, std::fminf(blend_in == 0.0f ? 1.0f : src->time / blend_in, blend_out == 0.0f ? 1.0f : (src->action->length - src->time) / blend_out)) * M_PI) * 0.5f + 0.5f;

			return true;
		};

		apl_list->layers.push_back(layer);

		NewEntity * sound_ent = new NewEntity();
		sound_ent->addComponent(new AudioComponent("data/assets/audio/ouch.wav"));
		sound_ent->addComponent(new PositionComponent(*mob->p));
		entity->world->AddEntity(sound_ent);

		mob->hit = false;
	}

	if (mob->landed)
		time_since_landed = 0.0f;
	else
		time_since_landed += dTime;

	vp = bu_blend(vp, mob->v, -4.0f, 1.0f, dTime);
	vb = bu_blend(vb, mob->v-vp, -4.0f, 1.0f, dTime);

	Vec3 v1 = mob->v-mob->land_v;
	Vec3 v2 = v1;
	v2 -= mob->up*mob->up.Dot(v2);
	float vl = v2.Len();
	if (vl!=0.0f)
		v2/=vl;

	float walk_blend_target_weight = std::fminf(1.0f, v1.Len()*200.0f/*/scale*/);

	walk_blend_apl->weight -= walk_blend_target_weight;
	float walk_dif = 10.0f*dTime;
	if (abs(walk_blend_apl->weight)<=walk_dif) {
		walk_blend_apl->weight = 0.0f;
	}
	else {
		if (walk_blend_apl->weight<0.0f)
			walk_blend_apl->weight += walk_dif;
		else
			walk_blend_apl->weight -= walk_dif;
	}
	walk_blend_apl->weight += walk_blend_target_weight;

	float arms_walk_target_blend = walk_blend_apl->weight * (0.5f + 0.5f * abs(v2.Dot(mob->facing))) * (mob->action == 0 ? 1.0f : 0.0f);
	arms_walk_blend_apl->weight -= arms_walk_target_blend;
	arms_walk_blend_apl->weight *= exp(log(0.0001f)*dTime);
	arms_walk_blend_apl->weight += arms_walk_target_blend;

	float idle_target_blend = mob->action == 0 ? 1.0f : 0.0f;
	idle_blend_apl->weight -= idle_target_blend;
	idle_blend_apl->weight *= exp(log(0.0001f)*dTime);
	idle_blend_apl->weight += idle_target_blend;

	float leg_move = std::fmaxf(0.0f, 1.0f - time_since_landed * 10.0f);
	v1.z *= leg_move;
	leg_move *= vl/**v2.Dot(Vec2(-sin(facing.x), cos(facing.x)))*//*/getSize()*/*60.0f;
	//leg_move /= scale;
	if (leg_move > 90.0f)
		leg_move = (leg_move-90.0f)*3.0f/4.0f+90.0f;
	if (leg_move > 120.0f)
		leg_move = (leg_move-120.0f)/2.0f+120.0f;

	run_blend_apl->weight = 0.5f;

	float back_blend_target = 0.0f;
	if (v2.Dot(mob->facing) < back_limit)
	{
		back_blend_target = 1.0f;
		back_limit = 0.0f;
	}
	else
	{
		back_limit = -0.1f;
	}

	back_blend_apl->weight -= back_blend_target;
	back_blend_apl->weight *= exp(log(0.0001f)*dTime);
	back_blend_apl->weight += back_blend_target;

	float lr = mob->move_facing.Cross(mob->up).Dot(v2);

	float prev_lr = sin(right_blend_apl->weight*M_PI/2.0f)-sin(left_blend_apl->weight*M_PI/2.0f);

	prev_lr -= lr;
	prev_lr *= exp(-8.0f*dTime);
	prev_lr += lr;

	right_blend_apl->weight = std::fmaxf(0.0f, asin(std::fminf(prev_lr, 1.0f)) / M_PI*2.0f);
	left_blend_apl->weight = std::fmaxf(0.0f, asin(std::fminf(-prev_lr, 1.0f)) / M_PI*2.0f);
	back_right_blend_apl->weight = std::fmaxf(0.0f, asin(std::fminf(prev_lr, 1.0f)) / M_PI*2.0f);
	back_left_blend_apl->weight = std::fmaxf(0.0f, asin(std::fminf(-prev_lr, 1.0f)) / M_PI*2.0f);

	float turn = mob->move_facing.Cross(mob->up).Dot(mob->facing);
	turn_left_src_apl->time = std::fmaxf(0.0f, asin(std::fminf(-turn, 1.0f)) / M_PI * 60.0f);
	turn_right_src_apl->time = std::fmaxf(0.0f, asin(std::fminf(turn, 1.0f)) / M_PI * 60.0f);

	walk_spd_apl->speed = leg_move;
	arms_walk_spd_apl->speed = leg_move;

	float fall_blend_target_weight = std::fminf(1.0f, std::fmaxf(0.0f, sqrt(time_since_landed * 5.0f) - 0.1f));
	fall_blend_apl->weight -= fall_blend_target_weight;
	float fall_dif = 30.0f*dTime;
	if (abs(fall_blend_apl->weight)<=fall_dif) {
		fall_blend_apl->weight = 0.0f;
	}
	else {
		if (fall_blend_apl->weight<0.0f)
			fall_blend_apl->weight += fall_dif;
		else
			fall_blend_apl->weight -= fall_dif;
	}
	fall_blend_apl->weight += fall_blend_target_weight;

	Vec3 lean = vb+mob->up*8.0f;
	if (lean.z<0.0f)
		lean.z=0.0f;
	lean /= lean.Len();
	Vec3 axis = mob->up.Cross(lean);

	float tilt_blend_weight = lean.Dot(mob->facing);

	if (tilt_blend_weight<0.0f/* || combat_timer<4.0f*/)
		tilt_blend_weight /= 8.0f;

	tilt_blend_apl->weight -= tilt_blend_weight;
	tilt_blend_apl->weight *= exp(log(0.0005f)*dTime);
	tilt_blend_apl->weight += tilt_blend_weight;
}

bool AnimationControlComponent::start_action(const std::string& fname)
{
	HitComponent * hit = entity->getComponent<HitComponent>();

	auto sr = Resource::get<StringResource>(fname);
	if (sr != 0)
	{
		Script scr(std::istringstream(sr->string));
		std::shared_ptr<ScriptMemory> mem(new ScriptMemory());
		scr.run(mem);

		std::string action = "";
		float speed = 1.0f;

		float blend_in = 0.0f;
		float blend_out = 0.0f;
		float start = 0.0f;
		float finish = 0.0f;
		float recover = 0.0f;

		mem->get("action", action);
		mem->get("speed", speed);

		mem->get("blend_in", blend_in);
		mem->get("blend_out", blend_out);
		mem->get("start", start);
		mem->get("finish", finish);
		mem->get("recover", recover);

		auto itAction = anim->actions.find(action);
		if (itAction != anim->actions.end())
		{
			auto src = std::make_shared<APLSource>(&itAction->second, false);
			auto layer = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 1.0f, std::make_shared<APLSpeed>(speed, src));

			std::weak_ptr<APLSource> w_src(src);
			std::weak_ptr<APLBlend> w_layer(layer);

			std::shared_ptr<ActionData> aData(new ActionData());
			mob->action = aData;

			src->on_tick = [=](float dTime) -> bool {
				auto src = w_src.lock();
				auto layer = w_layer.lock();
				layer->weight = -cos(std::fminf(1.0f, std::fminf(blend_in == 0.0f ? 1.0f : src->time / blend_in, blend_out == 0.0f ? 1.0f : (src->action->length - src->time) / blend_out)) * M_PI) * 0.5f + 0.5f;

				if (mob->action == aData)
				{
					if (src->time >= start)
					{
						if (hit != 0)
							hit->active = true;
					}
					if (src->time >= finish)
					{
						if (hit != 0)
						{
							hit->active = false;
							hit->hit.clear();
						}
					}

					if (src->time >= recover)
					{
						mob->action.reset();
					}
				}

				return true;
			};

			action_apl->layers.push_back(layer);

			return true;
		}
	}
	return false;
}

void AnimationControlComponent::tick(float dTime)
{
	if (anim == nullptr) {
		auto anim_loaded = Resource::get<SkeletalAnimation>("data/assets/units/player/KnightGuy.anim");
		if (anim_loaded != nullptr) {
			auto pc = entity->getComponent<PoseComponent>();
			if (pc != nullptr) {
				anim = anim_loaded;
				pc->pose = anim->armature;
				pose = &pc->pose;
				initAPL();
			}
		}
	}
	if (mob == nullptr)
		mob = entity->getComponent<MobComponent>();
	if (mob != nullptr && pose != nullptr) {
		/*{
			Timeslot timeslot_tick_apl("tick_apl");
			tickAPL(dTime);
		}
		{
			Timeslot timeslot_apl_tick("apl_tick");
			apl->tick(dTime, pose);
		}*/
		//pose->bones[0].transform = anim->armature.bones[0].transform;
		//pose->bones[0].transform *= Matrix3(mob->cam_facing.Cross(mob->up), mob->cam_facing, mob->up);
		//pose->bones[0].transform *= Matrix4::Translation(-mob->up * 1.25f);

		//Timeslot timeslot_pose_update("pose_update");

		//pose->update();
		auto g = entity->getComponent<GraphicsComponent>();
		g->decs.items[0]->local = Matrix4();
		Vec3 flat_facing = mob->cam_facing - mob->up * mob->up.Dot(mob->cam_facing);
		flat_facing.Normalize();
		g->decs.items[0]->local *= Matrix3(flat_facing.Cross(mob->up), flat_facing, mob->up);
		g->decs.items[0]->local *= Matrix4::Translation(-mob->up * 1.25f);
	}
}

void AnimationControlComponent::writeLog(outstream& os, ClientData& client)
{
	synchro.writeLog(os);
	os << cs;
}

void AnimationControlComponent::readLog(instream& is)
{
	synchro.readLog(is);
	is >> cs;
}

void AnimationControlComponent::writeLog(outstream& os)
{
}

void AnimationControlComponent::readLog(instream& is, ClientData& client)
{
}

void AnimationControlComponent::interpolate(Component * pComponent, float fWeight)
{
	auto acc = dynamic_cast<AnimationControlComponent*>(pComponent);
	if (acc != 0)
	{
		synchro.blend(acc->synchro);
		acc->cs.update(cs);
	}
}

void AnimationControlComponent::write_to(outstream& os, ClientData& client) const
{
	synchro.writeLog(os);
	os << cs;
}

void AnimationControlComponent::write_to(outstream& os) const
{
	synchro.writeLog(os);
	os << cs;
}
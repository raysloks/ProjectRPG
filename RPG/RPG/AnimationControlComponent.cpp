#include "AnimationControlComponent.h"

#include "NewEntity.h"

#include "World.h"

#include "Client.h"

#include "MobComponent.h"
#include "GraphicsComponent.h"
#include "PoseComponent.h"
#include "AudioComponent.h"
#include "CameraShakeComponent.h"
#include "PositionComponent.h"
#include "HitComponent.h"

#include "BlendUtility.h"

#include "AnimationState.h"
#include "SimpleState.h"
#include "CycleState.h"
#include "RunCycleState.h"

AutoSerialFactory<AnimationControlComponent, Component> AnimationControlComponent::_factory("AnimationControlComponent");

AnimationControlComponent::AnimationControlComponent(void) : Component(_factory.id)
{
	scale = 1.0f;
	sync = 0;
	state = nullptr;
}

AnimationControlComponent::AnimationControlComponent(instream& is, bool full) : Component(_factory.id)
{
	scale = 1.0f;
	sync = 0;
	state = nullptr;
}

AnimationControlComponent::~AnimationControlComponent(void)
{
	if (state)
		delete state;
	for (auto s : removed_states)
		delete s;
}

void AnimationControlComponent::connect(NewEntity * pEntity, bool authority)
{
}

void AnimationControlComponent::disconnect(void)
{
}

void AnimationControlComponent::tick(float dTime)
{
	auto p = entity->getComponent<PositionComponent>();
	auto g = entity->getComponent<GraphicsComponent>();
	auto mob = entity->getComponent<MobComponent>();
	auto pose = entity->getComponent<PoseComponent>();
	auto anim = Resource::get<SkeletalAnimation>(pose->anim);

	float prev_frame = pose->frame;

	if (anim)
	{
		overtime += dTime;
		do
		{
			if (!state)
			{
				auto run_cycle = new RunCycleState("run", 0.3f, "idle", 1.0f);
				if (entity->world->authority)
				{
					auto func = [=](int bid)
					{
						if (mob->landed)
						{
							NewEntity * sound_ent = new NewEntity();
							auto pos = new PositionComponent(p->p + Vec3() * anim->getMatrix(bid, pose->frame) * g->decs.items.front()->local);
							auto audio = new AudioComponent("data/assets/audio/step.wav");
							audio->gain = scale * 0.25f;
							sound_ent->addComponent(pos);
							sound_ent->addComponent(audio);
							entity->world->AddEntity(sound_ent);
						}
					};
					run_cycle->events.insert(std::make_pair(0.05f, std::bind(func, anim->getIndex("Foot_R"))));
					run_cycle->events.insert(std::make_pair(0.55f, std::bind(func, anim->getIndex("Foot_L"))));
				}
				set_state(run_cycle);
			}

			float added_time = overtime;
			overtime = 0.0f;
			if (state)
				state->tick(added_time);
		} while (overtime > 0.0f);

		auto simple_state = reinterpret_cast<SimpleState*>(state);

		if (has_state("run") && pose->anim == "data/assets/units/player/hoodlum.anim")
		{
			auto prev_pose = pose->pose;
			anim->getPose((20.0f - mob->facing.y * 20.0f / M_PI) * simple_state->blend_t + 10.0f * (1.0f - simple_state->blend_t), "look_vertical", pose->pose);
			pose->pose.add(prev_pose);
		}

		for (auto s : removed_states)
			delete s;
		removed_states.clear();

		transform = Matrix4::Translation(-root - mob->up - prev_root * (1.0f - simple_state->blend_t));
		transform *= Quaternion(M_PI - mob->facing.x, Vec3(0.0f, 0.0f, 1.0f));

		transform *= Matrix4::Scale(Vec3(scale));

		transform *= Matrix4::Translation(mob->up * (mob->r * 2.0f - 1.0f));
		
		for (auto dec : g->decs.items)
		{
			dec->local = transform;
		}

		/*if (mob->hit)
		{
			mob->hit = false;
			set_state(new SimpleState("hit", 4.0f));
		}*/

		pose->pose.update();
		Vec3 root_position = Vec3() * pose->pose.transforms[anim->getIndex("root")] - prev_root * (1.0f - simple_state->blend_t);
		Vec3 root_movement = (root_position - root) * Matrix3(transform);
		root = root_position;
		if (entity->world->authority)
		{
			mob->external_dp += root_movement;
		}
	}
}

void AnimationControlComponent::pre_frame(float dTime)
{
}

void AnimationControlComponent::writeLog(outstream& os, ClientData& client)
{
	os << scale;
	if (state)
	{
		AnimationState::_registry.serialize(os, state);
		state->write_to(os);
	}
	else
	{
		os << uint32_t(0);
	}
	os << sync;
}

void AnimationControlComponent::readLog(instream& is)
{
	is >> scale;
	auto factory = AnimationState::_registry.deserialize(is);
	if (factory)
		set_state(factory->create(is, false));
	else
		set_state(nullptr);
	is >> sync;
}

void AnimationControlComponent::writeLog(outstream& os)
{
}

void AnimationControlComponent::readLog(instream& is, ClientData& client)
{
}

void AnimationControlComponent::interpolate(Component * pComponent, float fWeight)
{
	auto other = reinterpret_cast<AnimationControlComponent*>(pComponent);
	
	if (SyncState::is_ordered_strict(sync, other->sync))
	{
		if (other->state)
			set_state(dynamic_cast<AnimationState*>(AnimationState::_registry.getFactory(other->state->getSerial())->create(other->state)));
		else
			set_state(nullptr);
		sync = other->sync;
	}
	else
	{
		if (state && other->state)
			state->interpolate(other->state, fWeight);
	}

	scale = bu_blend(other->scale, scale, fWeight);
}

void AnimationControlComponent::write_to(outstream& os, ClientData& client) const
{
}

void AnimationControlComponent::write_to(outstream& os) const
{
}

void AnimationControlComponent::set_state(AnimationState * new_state)
{
	++sync;
	auto simple_state = reinterpret_cast<SimpleState*>(state);
	if (simple_state)
	{
		prev_root = root + prev_root * (1.0f - simple_state->blend_t);
		root = Vec3();
	}
	if (new_state)
	{
		auto pose = entity->getComponent<PoseComponent>();
		auto mob = entity->getComponent<MobComponent>();

		new_state->acc = this;
		new_state->pose = pose;
		new_state->mob = mob;
		
		new_state->authority = entity->world->authority;
	}
	if (state)
	{
		state->leave(new_state);
		removed_states.push_back(state);
	}
	if (new_state)
		new_state->enter(state);
	state = new_state;
}

bool AnimationControlComponent::has_state(const std::string & name)
{
	SimpleState * simple = dynamic_cast<SimpleState*>(state);
	if (simple)
		return strncmp(simple->name.c_str(), name.c_str(), name.size()) == 0;
	return false;
}

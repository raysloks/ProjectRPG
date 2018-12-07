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

const AutoSerialFactory<AnimationControlComponent> AnimationControlComponent::_factory("AnimationControlComponent");

AnimationControlComponent::AnimationControlComponent(void) : Serializable(_factory.id)
{
	scale = 1.0f;
	sync = 0;
	state = nullptr;
}

AnimationControlComponent::AnimationControlComponent(instream& is, bool full) : Serializable(_factory.id)
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

	float prev_frame = pose->frame;

	overtime += dTime;
	do
	{
		if (!state)
		{
			auto run_cycle = new RunCycleState("run", 0.3f * 4.0f, "idle", 1.0f);
			if (entity->world->authority)
			{
				auto func = [=]()
				{
					NewEntity * sound_ent = new NewEntity();
					auto audio = new AudioComponent("data/assets/audio/step.wav");
					audio->gain = scale * 0.25f;
					audio->pos_id = entity->get_id();
					sound_ent->addComponent(audio);
					entity->world->AddEntity(sound_ent);
				};
				run_cycle->events.insert(std::make_pair(0.05f, func));
				run_cycle->events.insert(std::make_pair(0.55f, func));
			}
			set_state(run_cycle);
		}

		float added_time = overtime;
		overtime = 0.0f;
		if (state)
			state->tick(added_time);
	} while (overtime > 0.0f);

	for (auto s : removed_states)
		delete s;
	removed_states.clear();

	transform = Matrix4::Translation(-root - mob->up * 1.0f);
	transform *= Quaternion(M_PI - mob->facing.x, Vec3(0.0f, 0.0f, 1.0f));

	transform *= Matrix4::Scale(Vec3(scale, scale, scale));
			
	for (auto dec : g->decs.items)
	{
		dec->local = transform;
	}

	/*if (mob->hit)
	{
		mob->hit = false;
		set_state(new SimpleState("hit", 4.0f));
	}*/

	auto anim = Resource::get<SkeletalAnimation>(pose->anim);
	if (anim)
	{
		Vec3 root_position = Vec3() * anim->getMatrix(anim->getIndex("root"), pose->frame);
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
		Serializable::serialize(os, state);
		state->write_to(os, false);
	}
	else
	{
		os << (uint32_t)0;
	}
	os << sync;
}

void AnimationControlComponent::readLog(instream& is)
{
	is >> scale;
	auto factory = Serializable::deserialize(is);
	if (factory)
		set_state(dynamic_cast<AnimationState*>(factory->create(is, false)));
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
			set_state(dynamic_cast<AnimationState*>(Serializable::getFactory(other->state->getSerialID())->create(other->state)));
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
	root = Vec3();
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

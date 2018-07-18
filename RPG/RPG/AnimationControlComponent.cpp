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

	do
	{
		if (!state)
		{
			set_state(new RunCycleState("run", 0.3f, "idle", 1.0f));
		}

		float added_time = overtime;
		overtime = 0.0f;
		if (state)
			state->tick(dTime + added_time);
	} while (overtime > 0.0f);

	for (auto s : removed_states)
		delete s;
	removed_states.clear();

	if (g && mob && pose)
	{
		if (!g->decs.items.empty())
		{
			if (mob->facing == Vec3())
				mob->facing = Vec3(0.0f, 1.0f, 0.0f);
			Vec3 flat_facing = mob->facing - mob->up * mob->up.Dot(mob->facing);
			flat_facing.Normalize();

			Matrix4 transform = Matrix3(flat_facing.Cross(mob->up), flat_facing, mob->up);

			if (mob->temp_team == 1)
				scale = 3.0f;

			transform *= Matrix4::Scale(Vec3(scale, scale, scale));

			if (mob->crouch && mob->health.current > 0.0f) // temp
				transform.mtrx[2][2] *= 0.75f;

			transform *= Matrix4::Translation(-mob->up * (mob->crouch ? 0.75f : 1.25f));
			
			for (auto dec : g->decs.items)
			{
				dec->local = transform;
			}

			if (mob->hit)
			{
				mob->hit = false;
				set_state(new SimpleState("hit", 4.0f));

				if (entity->world->authority)
				{
					if (mob->temp_team == 0)
					{
						NewEntity * sound_ent = new NewEntity();
						auto audio = new AudioComponent("data/assets/audio/ouch.wav");
						audio->pos_id = entity->get_id();
						sound_ent->addComponent(audio);
						entity->world->AddEntity(sound_ent);
					}

					if (mob->temp_team == 1)
					{
						NewEntity * sound_ent = new NewEntity();
						auto audio = new AudioComponent("data/assets/audio/ZombieOuch.wav");
						audio->pos_id = entity->get_id();
						sound_ent->addComponent(audio);
						entity->world->AddEntity(sound_ent);
					}
				}
			}

			auto anim = Resource::get<SkeletalAnimation>(pose->anim);
			if (anim)
			{
				if (entity->world->authority)
				{
					if (anim->getProperty("Hand_R.active", pose->frame) > 0.5f)
					{
						GlobalPosition pos = *mob->p + Vec3() * anim->getMatrix(anim->getIndex("Hand_R"), pose->frame) * transform;
						float radius = 0.2f * scale;
						if (debug)
						{
							debug->p = pos;
							debug->update();
							auto g = debug->entity->getComponent<GraphicsComponent>();
							if (g)
							{
								g->decs.items.front()->local = Matrix4::Scale(Vec3(radius, radius, radius));
								g->decs.update(0);
							}
						}
						auto nearby_mobs = entity->world->GetNearestComponents<MobComponent>(pos, 0.5f + radius);
						MobComponent * other = nullptr;
						for each (auto nearby in nearby_mobs)
						{
							if (nearby.second->temp_team != mob->temp_team)
							{
								other = nearby.second;
							}
						}

						if (other != nullptr)
						{
							other->do_damage(1, entity->get_id());
							other->hit = true;
							Vec3 dif = *other->p - *mob->p;
							dif.Normalize();
							other->v = dif * 8.0f + Vec3(0.0f, 0.0f, 1.0f);
						}
					}
				}
			}
		}
	}
}

void AnimationControlComponent::pre_frame(float dTime)
{
}

void AnimationControlComponent::writeLog(outstream& os, ClientData& client)
{
	os << scale;
	Serializable::serialize(os, state);
	if (state)
		state->write_to(os, false);
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
	if (new_state)
	{
		auto pose = entity->getComponent<PoseComponent>();
		auto mob = entity->getComponent<MobComponent>();

		new_state->acc = this;
		new_state->pose = pose;
		new_state->mob = mob;
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
		return simple->name.compare(name) == 0;
	return false;
}

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

	if (mob->facing == Vec3())
		mob->facing = Vec3(0.0f, 1.0f, 0.0f);
	Vec3 flat_facing = mob->facing - mob->up * mob->up.Dot(mob->facing);
	flat_facing.Normalize();

	Matrix4 transform = Matrix4::Translation(-root);
	transform *= Matrix3(flat_facing.Cross(mob->up), flat_facing, mob->up);

	if (mob->temp_team == 1)
		scale = 3.0f;

	transform *= Matrix4::Scale(Vec3(scale, scale, scale));

	transform *= Matrix4::Translation(-mob->up * (mob->crouch ? 0.75f : 1.25f));
			
	for (auto dec : g->decs.items)
	{
		dec->local = transform;
	}

	if (mob->hit)
	{
		mob->hit = false;
		set_state(new SimpleState("hit", 4.0f));
	}

	auto anim = Resource::get<SkeletalAnimation>(pose->anim);
	if (anim)
	{
		Vec3 root_position = Vec3() * anim->getMatrix(anim->getIndex("root"), pose->frame);
		Vec3 root_movement = (root_position - root) * Matrix3(transform);
		root = root_position;
		if (entity->world->authority)
		{
			mob->external_dp += root_movement;

			float active = anim->getProperty("Hand_R.active", pose->frame);
			GlobalPosition pos = *mob->p + Vec3() * anim->getMatrix(anim->getIndex("Hand_R"), pose->frame) * transform;
			float radius = 0.2f * scale * active;
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
				auto h = debug->entity->getComponent<HitComponent>();
				if (h)
				{
					h->active = active > 0.5f;
					h->r = radius;
					h->func = [=](MobComponent * target, const Vec3& v)
					{
						target->do_damage(4, entity->get_id());
						target->hit = true;
						target->v += v * 0.2f;
					};
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
	root = Vec3();
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
		return strncmp(simple->name.c_str(), name.c_str(), name.size()) == 0;
	return false;
}

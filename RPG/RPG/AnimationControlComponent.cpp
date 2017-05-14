#include "AnimationControlComponent.h"

#include "NewEntity.h"

#include "World.h"

#include "MobComponent.h"
#include "GraphicsComponent.h"
#include "PoseComponent.h"
#include "AudioComponent.h"

const AutoSerialFactory<AnimationControlComponent> AnimationControlComponent::_factory("AnimationControlComponent");

AnimationControlComponent::AnimationControlComponent(void) : Serializable(_factory.id)
{
}

AnimationControlComponent::AnimationControlComponent(instream& is, bool full) : Serializable(_factory.id)
{
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

void AnimationControlComponent::tick(float dTime)
{
	auto g = entity->getComponent<GraphicsComponent>();
	auto mob = entity->getComponent<MobComponent>();
	auto pose = entity->getComponent<PoseComponent>();
	if (g && mob && pose)
	{
		if (!g->decs.items.empty())
		{
			g->decs.items[0]->local = Matrix4();
			if (mob->cam_facing == Vec3())
				mob->cam_facing = Vec3(0.0f, 1.0f, 0.0f);
			Vec3 flat_facing = mob->cam_facing - mob->up * mob->up.Dot(mob->cam_facing);
			flat_facing.Normalize();
			g->decs.items[0]->local *= Matrix3(flat_facing.Cross(mob->up), flat_facing, mob->up);
			g->decs.items[0]->local *= Matrix4::Translation(-mob->up * (mob->crouch ? 0.75f : 1.25f));

			if (mob->hit)
			{
				mob->hit = false;
				if (state == 2)
					set_state(4);

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

			switch (state)
			{
			case 0:
				set_state(2);
				break;
			case 1:
				pose->frame += dTime * 60.0f;
				if (pose->frame > 440.5f)
					pose->frame = 440.5f;
				break;
			case 2:
				pose->frame += dTime * 60.0f * Vec2(mob->v).Len();
				if (pose->frame > 1088.5f)
					pose->frame -= 90.0f;
				break;
			case 3:
				pose->frame += dTime * 60.0f;
				if (pose->frame > 34.5f)
					set_state(2);
				break;
			case 4:
				pose->frame += dTime * 60.0f;
				if (pose->frame > 472.5f)
					set_state(2);
				break;
			}
		}
	}
}

void AnimationControlComponent::writeLog(outstream& os, ClientData& client)
{
	os << state;
}

void AnimationControlComponent::readLog(instream& is)
{
	is >> state;
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
	set_state(other->state);
}

void AnimationControlComponent::write_to(outstream& os, ClientData& client) const
{
}

void AnimationControlComponent::write_to(outstream& os) const
{
}

void AnimationControlComponent::set_state(uint32_t new_state)
{
	if (new_state != state)
	{
		auto pose = entity->getComponent<PoseComponent>();
		if (pose)
		{
			switch (new_state)
			{
			case 0:
				break;
			case 1:
				pose->frame = 384.5f;
				break;
			case 2:
				pose->frame = 998.5f;
				break;
			case 3:
				pose->frame = 0.5f;
				break;
			case 4:
				pose->frame = 462.5f;
				break;
			}
		}
		state = new_state;
	}
}
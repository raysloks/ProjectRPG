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
	sync = pEntity->ss.allocate([](ClientData& client)
	{
		
	}, std::function<bool(ClientData&)>());
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
			if (mob->facing == Vec3())
				mob->facing = Vec3(0.0f, 1.0f, 0.0f);
			Vec3 flat_facing = mob->facing - mob->up * mob->up.Dot(mob->facing);
			flat_facing.Normalize();

			Matrix4 transform = Matrix3(flat_facing.Cross(mob->up), flat_facing, mob->up);

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
				if (state == 2)
					set_state(5);

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

			float prev_frame = pose->frame;
			auto anim = Resource::get<SkeletalAnimation>(pose->anim);
			if (anim)
			{
				switch (state)
				{
				case 0:
					set_state(1);
					break;
				case 1:
				{
					pose->frame += dTime * 30.0f;
					if (pose->frame >= anim->getEnd("idle"))
						pose->frame -= anim->getLength("idle");
					if (mob->move != Vec3())
						set_state(2);
					break;
				}
				case 2:
				{
					pose->frame += dTime * 30.0f * Vec2(mob->v).Len();
					float halfway = anim->getEnd("run") - anim->getLength("run") / 2.0f;
					if (pose->frame > halfway && prev_frame <= halfway && !mob->landed)
						pose->frame = halfway;
					if (pose->frame >= anim->getEnd("run"))
					{
						if (mob->landed)
							pose->frame -= anim->getLength("run");
						else
							pose->frame = anim->getEnd("run");
					}
					if (mob->move == Vec3() && mob->landed)
						set_state(1);
					if (mob->input.find("rolling") != mob->input.end())
						set_state(3);
					break;
				}
				case 3:
					pose->frame += dTime * 120.0f;
					if (pose->frame > anim->getEnd("roll"))
						set_state(2);
					break;
				case 4:
					pose->frame += dTime * 240.0f;
					if (pose->frame > anim->getEnd("attack"))
						set_state(2);
					break;
				case 5:
					pose->frame += dTime * 60.0f;
					if (pose->frame > anim->getEnd("hit"))
						set_state(2);
					break;
				default:
					break;
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
			auto anim = Resource::get<SkeletalAnimation>(pose->anim);
			if (anim)
			{
				switch (new_state)
				{
				case 0:
					break;
				case 1:
					pose->frame = anim->getStart("idle");
					break;
				case 2:
					pose->frame = anim->getStart("run");
					break;
				case 3:
					pose->frame = anim->getStart("roll");
					break;
				case 4:
					pose->frame = anim->getStart("attack");
					break;
				case 5:
					pose->frame = anim->getStart("hit");
					break;
				default:
					break;
				}
			}
		}
		state = new_state;
	}
}
#include "AnimationControlComponent.h"

#include "NewEntity.h"

#include "MobComponent.h"
#include "GraphicsComponent.h"
#include "PoseComponent.h"

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
			g->decs.items[0]->local *= Matrix4::Translation(-mob->up * 1.25f);

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
				pose->frame += dTime * 60.0f * mob->v.Dot(mob->cam_facing);
				if (pose->frame > 1088.5f)
					pose->frame -= 90.0f;
				break;
			}
		}
	}
}

void AnimationControlComponent::writeLog(outstream& os, ClientData& client)
{
}

void AnimationControlComponent::readLog(instream& is)
{
}

void AnimationControlComponent::writeLog(outstream& os)
{
}

void AnimationControlComponent::readLog(instream& is, ClientData& client)
{
}

void AnimationControlComponent::interpolate(Component * pComponent, float fWeight)
{
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
			}
		}
		state = new_state;
	}
}
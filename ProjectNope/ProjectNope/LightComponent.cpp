#include "LightComponent.h"

#include <algorithm>

#include "NewEntity.h"
#include "PositionComponent.h"
#include "PoseComponent.h"

#include "SkeletalAnimation.h"

AutoSerialFactory<LightComponent, Component> LightComponent::_factory("LightComponent");

std::vector<LightComponent*> LightComponent::all;

LightComponent::LightComponent(void) : Component(_factory.id)
{
	all.push_back(this);
	pose = nullptr;
}

LightComponent::LightComponent(instream& is, bool full) : Component(_factory.id)
{
	all.push_back(this);
	pose = nullptr;
	is >> bone_id;
}

LightComponent::~LightComponent(void)
{
	all.erase(std::remove(all.begin(), all.end(), this), all.end());
}

void LightComponent::connect(NewEntity * pEntity, bool authority)
{
}

void LightComponent::disconnect(void)
{
}

void LightComponent::pre_frame(float dTime)
{
	/*PositionComponent * pc = entity->getComponent<PositionComponent>();
	if (pc != nullptr)
	{
		p = pc->p;

		if (pose == nullptr)
		{
			PoseComponent * pose_comp = entity->getComponent<PoseComponent>();
			if (pose_comp != nullptr)
				pose = &pose_comp->pose;
		}

		if (pose != nullptr)
		{
			if (pose->bones.size() > bone_id)
				p += Vec3() * pose->bones[bone_id].getTransform();
		}
	}*/
}

void LightComponent::tick(float dTime)
{
}

void LightComponent::writeLog(outstream& os, ClientData& client)
{
}

void LightComponent::readLog(instream& is)
{
}

void LightComponent::writeLog(outstream& os)
{
}

void LightComponent::readLog(instream& is, ClientData& client)
{
}

void LightComponent::interpolate(Component * pComponent, float fWeight)
{
}

void LightComponent::write_to(outstream& os, ClientData& client) const
{
	os << bone_id;
}

void LightComponent::write_to(outstream& os) const
{
}
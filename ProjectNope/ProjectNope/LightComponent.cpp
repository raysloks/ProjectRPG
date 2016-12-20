#include "LightComponent.h"

#include <algorithm>

#include "NewEntity.h"
#include "PositionComponent.h"

#include "SkeletalAnimation.h"

const AutoSerialFactory<LightComponent> LightComponent::_factory("LightComponent");

std::vector<LightComponent*> LightComponent::all;

LightComponent::LightComponent(void) : Serializable(_factory.id)
{
	all.push_back(this);
}

LightComponent::LightComponent(instream& is, bool full) : Serializable(_factory.id)
{
	all.push_back(this);
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

void LightComponent::frame(float dTime)
{
	PositionComponent * pc = entity->getComponent<PositionComponent>();
	if (pc != nullptr)
	{
		p = pc->p;

		if (pose != nullptr)
		{
			if (*pose != nullptr)
			{
				if ((*pose)->bones.size() > bone_id)
					p += Vec3() * (*pose)->bones[bone_id].getTransform();
			}
		}
	}
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
}

void LightComponent::write_to(outstream& os) const
{
}
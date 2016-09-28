#include "VoiceComponent.h"

const AutoSerialFactory<VoiceComponent> VoiceComponent::_factory("VoiceComponent");

VoiceComponent::VoiceComponent(void) : Serializable(_factory.id)
{
	before.reserve(2000);
	for (int i=0;i<2000;++i)
	{
		before.push_back(sin(float(i)*(2.0f*M_PI)/50.0f));
	}
}

VoiceComponent::VoiceComponent(instream& is, bool full) : Serializable(_factory.id)
{
}

VoiceComponent::~VoiceComponent(void)
{
}

void VoiceComponent::connect(NewEntity * pEntity, bool authority)
{
}

void VoiceComponent::disconnect(void)
{
}

#include <iostream>

void VoiceComponent::tick(float dTime)
{
	if (after.size()==0)
	{
		after.reserve(before.size());
		for (auto k=before.begin();k!=before.end();++k)
		{
			Vec2 val;
			for (auto n=before.begin();n!=before.end();++n)
			{
				float angle = -2.0f*M_PI*std::distance(before.begin(), k)*std::distance(before.begin(), n)*0.1f/before.size();
				val += Vec2(cos(angle), sin(angle))**n;
			}
			after.push_back(val);
		}
		auto max_val = after.begin();
		for (auto k=after.begin();k!=after.end();++k)
		{
			if (k->x*k->x+k->y*k->y>max_val->x*max_val->x+max_val->y*max_val->y)
				max_val = k;
		}
		std::cout << std::distance(after.begin(), max_val) << ": " << max_val->x << " " << max_val->y << std::endl;
	}
}

void VoiceComponent::writeLog(outstream& os, ClientData& client)
{
}

void VoiceComponent::readLog(instream& is)
{
}

void VoiceComponent::writeLog(outstream& os)
{
}

void VoiceComponent::readLog(instream& is, ClientData& client)
{
}

void VoiceComponent::interpolate(Component * pComponent, float fWeight)
{
}

void VoiceComponent::write_to(outstream& os, ClientData& client) const
{
}

void VoiceComponent::write_to(outstream& os) const
{
}
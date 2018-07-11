#include "PoseComponent.h"

const AutoSerialFactory<PoseComponent> PoseComponent::_factory("PoseComponent");

PoseComponent::PoseComponent(void) : Serializable(_factory.id)
{
}

PoseComponent::PoseComponent(instream& is, bool full) : Serializable(_factory.id)
{
	is >> anim;
}

PoseComponent::~PoseComponent(void)
{
}

void PoseComponent::connect(NewEntity * pEntity, bool authority)
{
}

void PoseComponent::disconnect(void)
{
}

void PoseComponent::pre_frame(float dTime)
{
}

void PoseComponent::tick(float dTime)
{
}

void PoseComponent::writeLog(outstream& os, ClientData& client)
{
}

void PoseComponent::readLog(instream& is)
{
}

void PoseComponent::writeLog(outstream& os)
{
}

void PoseComponent::readLog(instream& is, ClientData& client)
{
}

void PoseComponent::interpolate(Component * pComponent, float fWeight)
{
}

void PoseComponent::write_to(outstream& os, ClientData& client) const
{
	os << anim;
}

void PoseComponent::write_to(outstream& os) const
{
	os << anim;
}
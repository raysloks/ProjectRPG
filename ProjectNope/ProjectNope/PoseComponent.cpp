#include "PoseComponent.h"

AutoSerialFactory<PoseComponent, Component> PoseComponent::_factory("PoseComponent");

PoseComponent::PoseComponent(void) : Component(_factory.id)
{
}

PoseComponent::PoseComponent(instream& is) : Component(_factory.id)
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

void PoseComponent::writeLog(outstream& os, const std::shared_ptr<ClientData>& client)
{
}

void PoseComponent::readLog(instream& is)
{
}

void PoseComponent::writeLog(outstream& os)
{
}

void PoseComponent::readLog(instream& is, const std::shared_ptr<ClientData>& client)
{
}

void PoseComponent::interpolate(Component * pComponent, float fWeight)
{
}

void PoseComponent::write_to(outstream& os, const std::shared_ptr<ClientData>& client) const
{
	os << anim;
}

void PoseComponent::write_to(outstream& os) const
{
	os << anim;
}
#include "InteractComponent.h"

AutoSerialFactory<InteractComponent, Component> InteractComponent::_factory("InteractComponent");

InteractComponent::InteractComponent(void) : Component(_factory.id)
{
}

InteractComponent::InteractComponent(instream& is) : Component(_factory.id)
{
	is >> name >> action_name;
}

InteractComponent::~InteractComponent(void)
{
}

void InteractComponent::connect(NewEntity * pEntity, bool authority)
{
}

void InteractComponent::disconnect(void)
{
}

void InteractComponent::pre_frame(float dTime)
{
}

void InteractComponent::post_frame(float dTime)
{
}

void InteractComponent::tick(float dTime)
{
}

void InteractComponent::writeLog(outstream& os, const std::shared_ptr<ClientData>& client)
{
}

void InteractComponent::readLog(instream& is)
{
}

void InteractComponent::writeLog(outstream& os)
{
}

void InteractComponent::readLog(instream& is, const std::shared_ptr<ClientData>& client)
{
}

void InteractComponent::interpolate(Component * pComponent, float fWeight)
{
}

void InteractComponent::write_to(outstream& os, const std::shared_ptr<ClientData>& client) const
{
	os << name << action_name;
}

void InteractComponent::write_to(outstream& os) const
{
}
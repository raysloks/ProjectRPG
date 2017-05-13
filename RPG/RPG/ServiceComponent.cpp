#include "ServiceComponent.h"

const AutoSerialFactory<ServiceComponent> ServiceComponent::_factory("ServiceComponent");

ServiceComponent::ServiceComponent(void) : Serializable(_factory.id)
{
}

ServiceComponent::ServiceComponent(instream& is, bool full) : Serializable(_factory.id)
{
}

ServiceComponent::~ServiceComponent(void)
{
}

void ServiceComponent::connect(NewEntity * pEntity, bool authority)
{
}

void ServiceComponent::disconnect(void)
{
}

void ServiceComponent::pre_frame(float dTime)
{
}

void ServiceComponent::post_frame(float dTime)
{
}

void ServiceComponent::tick(float dTime)
{
	if (on_tick)
		on_tick(dTime);
}

void ServiceComponent::writeLog(outstream& os, ClientData& client)
{
}

void ServiceComponent::readLog(instream& is)
{
}

void ServiceComponent::writeLog(outstream& os)
{
}

void ServiceComponent::readLog(instream& is, ClientData& client)
{
}

void ServiceComponent::interpolate(Component * pComponent, float fWeight)
{
}

void ServiceComponent::write_to(outstream& os, ClientData& client) const
{
}

void ServiceComponent::write_to(outstream& os) const
{
}
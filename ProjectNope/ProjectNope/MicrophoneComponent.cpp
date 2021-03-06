#include "MicrophoneComponent.h"

AutoSerialFactory<MicrophoneComponent, Component> MicrophoneComponent::_factory("MicrophoneComponent");

MicrophoneComponent::MicrophoneComponent(void) : Component(_factory.id)
{
}

MicrophoneComponent::MicrophoneComponent(instream& is) : Component(_factory.id)
{
}

MicrophoneComponent::~MicrophoneComponent(void)
{
}

void MicrophoneComponent::connect(NewEntity * pEntity, bool authority)
{
}

void MicrophoneComponent::disconnect(void)
{
}

void MicrophoneComponent::tick(float dTime)
{
}

void MicrophoneComponent::writeLog(outstream& os, const std::shared_ptr<ClientData>& client)
{
}

void MicrophoneComponent::readLog(instream& is)
{
}

void MicrophoneComponent::writeLog(outstream& os)
{
}

void MicrophoneComponent::readLog(instream& is, const std::shared_ptr<ClientData>& client)
{
}

void MicrophoneComponent::interpolate(Component * pComponent, float fWeight)
{
}

void MicrophoneComponent::write_to(outstream& os, const std::shared_ptr<ClientData>& client) const
{
}

void MicrophoneComponent::write_to(outstream& os) const
{
}
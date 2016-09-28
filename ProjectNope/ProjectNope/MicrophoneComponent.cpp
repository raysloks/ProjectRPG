#include "MicrophoneComponent.h"

const AutoSerialFactory<MicrophoneComponent> MicrophoneComponent::_factory("MicrophoneComponent");

MicrophoneComponent::MicrophoneComponent(void) : Serializable(_factory.id)
{
}

MicrophoneComponent::MicrophoneComponent(instream& is, bool full) : Serializable(_factory.id)
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

void MicrophoneComponent::writeLog(outstream& os, ClientData& client)
{
}

void MicrophoneComponent::readLog(instream& is)
{
}

void MicrophoneComponent::writeLog(outstream& os)
{
}

void MicrophoneComponent::readLog(instream& is, ClientData& client)
{
}

void MicrophoneComponent::interpolate(Component * pComponent, float fWeight)
{
}

void MicrophoneComponent::write_to(outstream& os, ClientData& client) const
{
}

void MicrophoneComponent::write_to(outstream& os) const
{
}
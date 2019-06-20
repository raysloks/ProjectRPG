#include "SpriteComponent.h"

AutoSerialFactory<SpriteComponent, Component> SpriteComponent::_factory("SpriteComponent");

SpriteComponent::SpriteComponent(void) : Component(_factory.id)
{
}

SpriteComponent::SpriteComponent(instream& is) : Component(_factory.id)
{
}

SpriteComponent::~SpriteComponent(void)
{
}

void SpriteComponent::connect(NewEntity * pEntity, bool authority)
{
}

void SpriteComponent::disconnect(void)
{
}

void SpriteComponent::pre_frame(float dTime)
{
}

void SpriteComponent::tick(float dTime)
{
}

void SpriteComponent::writeLog(outstream& os, const std::shared_ptr<ClientData>& client)
{
}

void SpriteComponent::readLog(instream& is)
{
}

void SpriteComponent::writeLog(outstream& os)
{
}

void SpriteComponent::readLog(instream& is, const std::shared_ptr<ClientData>& client)
{
}

void SpriteComponent::interpolate(Component * pComponent, float fWeight)
{
}

void SpriteComponent::write_to(outstream& os, const std::shared_ptr<ClientData>& client) const
{
}

void SpriteComponent::write_to(outstream& os) const
{
}
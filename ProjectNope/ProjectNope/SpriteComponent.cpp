#include "SpriteComponent.h"

ASF_C(SpriteComponent, Component)

SpriteComponent::SpriteComponent(void) : Component(_factory.id)
{
}

SpriteComponent::SpriteComponent(instream& is, bool full) : Component(_factory.id)
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

void SpriteComponent::writeLog(outstream& os, ClientData& client)
{
}

void SpriteComponent::readLog(instream& is)
{
}

void SpriteComponent::writeLog(outstream& os)
{
}

void SpriteComponent::readLog(instream& is, ClientData& client)
{
}

void SpriteComponent::interpolate(Component * pComponent, float fWeight)
{
}

void SpriteComponent::write_to(outstream& os, ClientData& client) const
{
}

void SpriteComponent::write_to(outstream& os) const
{
}
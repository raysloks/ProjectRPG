#include "RigidBodyComponent.h"

#include <set>
#include "NewEntity.h"
#include "World.h"

AutoSerialFactory<RigidBodyComponent, Component> RigidBodyComponent::_factory("RigidBodyComponent");

RigidBodyComponent::RigidBodyComponent(void) : Component(_factory.id)
{
}

RigidBodyComponent::RigidBodyComponent(instream& is) : Component(_factory.id)
{
}

RigidBodyComponent::~RigidBodyComponent(void)
{
}

void RigidBodyComponent::connect(NewEntity * pEntity, bool authority)
{
}

void RigidBodyComponent::disconnect(void)
{
}

void RigidBodyComponent::tick(float dTime)
{
}

void RigidBodyComponent::writeLog(outstream& os, const std::shared_ptr<ClientData>& client)
{
}

void RigidBodyComponent::readLog(instream& is)
{
}

void RigidBodyComponent::writeLog(outstream& os)
{
}

void RigidBodyComponent::readLog(instream& is, const std::shared_ptr<ClientData>& client)
{
}

void RigidBodyComponent::interpolate(Component * pComponent, float fWeight)
{
}

void RigidBodyComponent::write_to(outstream& os, const std::shared_ptr<ClientData>& client) const
{
}

void RigidBodyComponent::write_to(outstream& os) const
{
}
#include "RigidBodyComponent.h"

#include <set>
#include "NewEntity.h"
#include "World.h"

const AutoSerialFactory<RigidBodyComponent> RigidBodyComponent::_factory("RigidBodyComponent");

RigidBodyComponent::RigidBodyComponent(void) : Serializable(_factory.id)
{
}

RigidBodyComponent::RigidBodyComponent(instream& is, bool full) : Serializable(_factory.id)
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

void RigidBodyComponent::writeLog(outstream& os, ClientData& client)
{
}

void RigidBodyComponent::readLog(instream& is)
{
}

void RigidBodyComponent::writeLog(outstream& os)
{
}

void RigidBodyComponent::readLog(instream& is, ClientData& client)
{
}

void RigidBodyComponent::interpolate(Component * pComponent, float fWeight)
{
}

void RigidBodyComponent::write_to(outstream& os, ClientData& client) const
{
}

void RigidBodyComponent::write_to(outstream& os) const
{
}
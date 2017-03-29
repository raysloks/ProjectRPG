#include "WeaponComponent.h"

const AutoSerialFactory<WeaponComponent> WeaponComponent::_factory("WeaponComponent");

WeaponComponent::WeaponComponent(void) : Serializable(_factory.id)
{
}

WeaponComponent::WeaponComponent(instream& is, bool full) : Serializable(_factory.id)
{
}

WeaponComponent::~WeaponComponent(void)
{
}

void WeaponComponent::connect(NewEntity * pEntity, bool authority)
{
}

void WeaponComponent::disconnect(void)
{
}

void WeaponComponent::pre_frame(float dTime)
{
}

void WeaponComponent::tick(float dTime)
{
}

void WeaponComponent::writeLog(outstream& os, ClientData& client)
{
}

void WeaponComponent::readLog(instream& is)
{
}

void WeaponComponent::writeLog(outstream& os)
{
}

void WeaponComponent::readLog(instream& is, ClientData& client)
{
}

void WeaponComponent::interpolate(Component * pComponent, float fWeight)
{
}

void WeaponComponent::write_to(outstream& os, ClientData& client) const
{
}

void WeaponComponent::write_to(outstream& os) const
{
}
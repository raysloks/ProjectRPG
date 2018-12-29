#include "ExampleComponent.h"

AutoSerialFactory<ExampleComponent, Component> ExampleComponent::_factory("ExampleComponent");

ExampleComponent::ExampleComponent(void) : Component(_factory.id)
{
}

ExampleComponent::ExampleComponent(instream& is, bool full) : Component(_factory.id)
{
}

ExampleComponent::~ExampleComponent(void)
{
}

void ExampleComponent::connect(NewEntity * pEntity, bool authority)
{
}

void ExampleComponent::disconnect(void)
{
}

void ExampleComponent::pre_frame(float dTime)
{
}

void ExampleComponent::post_frame(float dTime)
{
}

void ExampleComponent::tick(float dTime)
{
}

void ExampleComponent::writeLog(outstream& os, ClientData& client)
{
}

void ExampleComponent::readLog(instream& is)
{
}

void ExampleComponent::writeLog(outstream& os)
{
}

void ExampleComponent::readLog(instream& is, ClientData& client)
{
}

void ExampleComponent::interpolate(Component * pComponent, float fWeight)
{
}

void ExampleComponent::write_to(outstream& os, ClientData& client) const
{
}

void ExampleComponent::write_to(outstream& os) const
{
}
#include "LearningComponent.h"

AutoSerialFactory<LearningComponent, Component> LearningComponent::_factory("LearningComponent");

LearningComponent::LearningComponent(void) : Component(_factory.id)
{
}

LearningComponent::LearningComponent(instream& is) : Component(_factory.id)
{
}

LearningComponent::~LearningComponent(void)
{
}

void LearningComponent::connect(NewEntity * pEntity, bool authority)
{
}

void LearningComponent::disconnect(void)
{
}

void LearningComponent::pre_frame(float dTime)
{
}

void LearningComponent::post_frame(float dTime)
{
}

void LearningComponent::tick(float dTime)
{
}

void LearningComponent::writeLog(outstream& os, const std::shared_ptr<ClientData>& client)
{
}

void LearningComponent::readLog(instream& is)
{
}

void LearningComponent::writeLog(outstream& os)
{
}

void LearningComponent::readLog(instream& is, const std::shared_ptr<ClientData>& client)
{
}

void LearningComponent::interpolate(Component * pComponent, float fWeight)
{
}

void LearningComponent::write_to(outstream& os, const std::shared_ptr<ClientData>& client) const
{
}

void LearningComponent::write_to(outstream& os) const
{
}
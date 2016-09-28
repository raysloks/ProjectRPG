#include "Component.h"

const AutoSerialFactory<Component> Component::_factory("Component");

Component::Component(void) : Serializable(_factory.id) {}

Component::Component(instream& is, bool full) : Serializable(_factory.id) {}

Component::~Component(void) {}

void Component::connect(NewEntity * pEntity, bool authority) {}

void Component::disconnect(void) {}

void Component::writeLog(outstream& os, ClientData& client) {}

void Component::readLog(instream& is) {}

void Component::writeLog(outstream& os) {}

void Component::readLog(instream& is, ClientData& client) {}

void Component::frame(float dTime){}

void Component::tick(float dTime){}

void Component::interpolate(Component * pComponent, float fWeight) {}

void Component::write_to(outstream& os, ClientData& client) const {}

void Component::write_to(outstream& os) const {}

bool Component::visible(ClientData& client) const { return true; }
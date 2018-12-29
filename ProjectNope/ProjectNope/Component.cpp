#include "Component.h"

SerializableRegistry<Component> Component::_registry;

Component::Component(uint32_t id) : _serial_id(id) {}

Component::~Component(void) {}

void Component::connect(NewEntity * pEntity, bool authority) {}

void Component::disconnect(void) {}

void Component::writeLog(outstream& os, ClientData& client) {}

void Component::readLog(instream& is) {}

void Component::writeLog(outstream& os) {}

void Component::readLog(instream& is, ClientData& client) {}

void Component::pre_frame(float dTime) {}

void Component::post_frame(float dTime) {}

void Component::tick(float dTime) {}

void Component::interpolate(Component * pComponent, float fWeight) {}

void Component::write_to(outstream& os, ClientData& client) const {}

void Component::write_to(outstream& os) const {}

bool Component::visible(ClientData& client) const { return true; }
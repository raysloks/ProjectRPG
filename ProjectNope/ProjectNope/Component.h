#ifndef COMPONENT_H
#define COMPONENT_H

#include "AutoSerialFactory.h"
#include "SerializableRegistry.h"

class NewEntity;
class ClientData;

class Component
{
public:
	Component(uint32_t id);
	virtual ~Component(void);

	virtual void connect(NewEntity * pEntity, bool authority);
	virtual void disconnect(void);

	virtual void writeLog(outstream& os, ClientData& client);
	virtual void readLog(instream& is);
	
	virtual void writeLog(outstream& os);
	virtual void readLog(instream& is, ClientData& client);

	virtual void pre_frame(float dTime);
	virtual void post_frame(float dTime);
	virtual void tick(float dTime);

	virtual void interpolate(Component * pComponent, float fWeight);

	virtual void write_to(outstream& os, ClientData& client) const;
	virtual void write_to(outstream& os) const;

	virtual bool visible(ClientData& client) const;

	virtual uint32_t getSerial() const;

	uint32_t _serial_id;

	static SerializableRegistry<Component> _registry;

	NewEntity * entity;
};

#endif
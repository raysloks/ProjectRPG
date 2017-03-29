#ifndef COMPONENT_H
#define COMPONENT_H

#include "Serializable.h"
#include "AutoSerialFactory.h"

class NewEntity;
class ClientData;

class Component :
	public virtual Serializable
{
public:
	Component(void);
	Component(instream& is, bool full);
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

	NewEntity * entity;

	static const AutoSerialFactory<Component> _factory;
};

#endif
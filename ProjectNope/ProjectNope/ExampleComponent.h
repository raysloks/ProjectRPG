#ifndef EXAMPLE_COMPONENT_H
#define EXAMPLE_COMPONENT_H

#include "Component.h"

class ExampleComponent :
	public Component
{
public:
	ExampleComponent(void);
	ExampleComponent(instream& is, bool full);
	~ExampleComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, ClientData& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	static const AutoSerialFactory<ExampleComponent> _factory;
};

#endif
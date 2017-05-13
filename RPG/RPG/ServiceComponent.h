#ifndef SERVICE_COMPONENT_H
#define SERVICE_COMPONENT_H

#include "Component.h"

#include <functional>

class ServiceComponent :
	public Component
{
public:
	ServiceComponent(void);
	ServiceComponent(instream& is, bool full);
	~ServiceComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void pre_frame(float dTime);
	void post_frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, ClientData& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	static const AutoSerialFactory<ServiceComponent> _factory;

	std::function<void(float)> on_tick;
};

#endif
#ifndef MICROPHONE_COMPONENT_H
#define MICROPHONE_COMPONENT_H

#include "Component.h"

class MicrophoneComponent :
	public Component
{
public:
	MicrophoneComponent(void);
	MicrophoneComponent(instream& is);
	~MicrophoneComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void tick(float dTime);

	void writeLog(outstream& os, const std::shared_ptr<ClientData>& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, const std::shared_ptr<ClientData>& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, const std::shared_ptr<ClientData>& client) const;
	void write_to(outstream& os) const;

	static AutoSerialFactory<MicrophoneComponent, Component> _factory;
};

#endif
#ifndef VOICE_COMPONENT_H
#define VOICE_COMPONENT_H

#include "Component.h"

#include "Vec2.h"

class VoiceComponent :
	public Component
{
public:
	VoiceComponent(void);
	VoiceComponent(instream& is);
	~VoiceComponent(void);

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

	static AutoSerialFactory<VoiceComponent, Component> _factory;

	std::vector<float> before;
	std::vector<Vec2> after;
};

#endif
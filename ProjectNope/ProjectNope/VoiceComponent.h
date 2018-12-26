#ifndef VOICE_COMPONENT_H
#define VOICE_COMPONENT_H

#include "Component.h"

#include "Vec2.h"

class VoiceComponent :
	public Component
{
public:
	VoiceComponent(void);
	VoiceComponent(instream& is, bool full);
	~VoiceComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void tick(float dTime);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, ClientData& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	ASF_H(VoiceComponent, Component)

	std::vector<float> before;
	std::vector<Vec2> after;
};

#endif
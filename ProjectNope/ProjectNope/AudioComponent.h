#ifndef AUDIO_COMPONENT_H
#define AUDIO_COMPONENT_H

#include "Component.h"

class AudioSource;
class PositionComponent;

class AudioComponent :
	public Component
{
public:
	AudioComponent(const std::string& sound, float gain = 1.0f);
	AudioComponent(instream& is, bool full);
	~AudioComponent(void);

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

	static const AutoSerialFactory<AudioComponent> _factory;

	PositionComponent * p;
	std::string _sound;
	AudioSource * src;
	float offset, gain;
};

#endif
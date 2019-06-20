#ifndef AUDIO_COMPONENT_H
#define AUDIO_COMPONENT_H

#include "Component.h"

#include "EntityID.h"
#include "GlobalPosition.h"

class AudioSource;
class PositionComponent;

class AudioComponent :
	public Component
{
public:
	AudioComponent(const std::string& sound, float gain = 1.0f);
	AudioComponent(instream& is);
	~AudioComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void pre_frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, const std::shared_ptr<ClientData>& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, const std::shared_ptr<ClientData>& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, const std::shared_ptr<ClientData>& client) const;
	void write_to(outstream& os) const;
	
	static AutoSerialFactory<AudioComponent, Component> _factory;

	std::string _sound;
	AudioSource * src;
	float offset, gain;

	EntityID pos_id;
};

#endif
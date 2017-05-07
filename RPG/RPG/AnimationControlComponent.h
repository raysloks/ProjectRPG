#ifndef ANIMATION_CONTROL_COMPONENT_H
#define ANIMATION_CONTROL_COMPONENT_H

#include "Component.h"

class AnimationControlComponent :
	public Component
{
public:
	AnimationControlComponent(void);
	AnimationControlComponent(instream& is, bool full);
	~AnimationControlComponent(void);

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

	static const AutoSerialFactory<AnimationControlComponent> _factory;

	void set_state(uint32_t new_state);

	uint32_t state;
};

#endif
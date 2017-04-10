#ifndef PLAYER_INPUT_COMPONENT_H
#define PLAYER_INPUT_COMPONENT_H

#include "Component.h"

#include "Quaternion.h"
#include "Vec3.h"
#include "Vec2.h"
#include "GlobalPosition.h"

#include "ControlState.h"

class MobComponent;

class PlayerInputComponent :
	public Component
{
public:
	PlayerInputComponent(void);
	PlayerInputComponent(instream& is, bool full);
	~PlayerInputComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void post_frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, ClientData& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	bool visible(ClientData& client) const;

	static const AutoSerialFactory<PlayerInputComponent> _factory;

	uint32_t client_id;

	MobComponent * mob = nullptr;

	GlobalPosition * p = nullptr;

	Vec3 move;
	Quaternion cam_rot;
	ControlState cs;
	bool run;
};

#endif
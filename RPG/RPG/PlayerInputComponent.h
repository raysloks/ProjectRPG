#ifndef PLAYER_INPUT_COMPONENT_H
#define PLAYER_INPUT_COMPONENT_H

#include "Component.h"

#include "Quaternion.h"
#include "Vec3.h"
#include "Vec2.h"
#include "GlobalPosition.h"

#include "SyncQueue.h"

class MobComponent;

class PlayerInputComponent :
	public Component
{
public:
	PlayerInputComponent(void);
	PlayerInputComponent(instream& is);
	~PlayerInputComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void post_frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, const std::shared_ptr<ClientData>& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, const std::shared_ptr<ClientData>& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, const std::shared_ptr<ClientData>& client) const;
	void write_to(outstream& os) const;

	bool visible(ClientData& client) const;

	static AutoSerialFactory<PlayerInputComponent, Component> _factory;

	uint32_t client_id;

	MobComponent * mob = nullptr;

	Vec2 facing;
	Vec3 move, move_space;
	SyncQueue<std::string> sc;
};

#endif
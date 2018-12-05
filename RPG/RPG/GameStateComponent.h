#ifndef GAME_STATE_COMPONENT_H
#define GAME_STATE_COMPONENT_H

#include "Component.h"

#include <memory>
#include <functional>
#include <unordered_map>

class RenderSetup;
class MobComponent;

enum GameState
{
	gameState_Setup,
	gameState_Ongoing,
	gameState_Over
};

class GameStateComponent :
	public Component
{
public:
	GameStateComponent(void);
	GameStateComponent(instream& is, bool full);
	~GameStateComponent(void);

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

	static const AutoSerialFactory<GameStateComponent> _factory;

	MobComponent * createAvatar(uint32_t client_id, uint32_t team, uint32_t index);

	void set_display(bool enable);

	std::shared_ptr<std::function<void(RenderSetup&)>> func;

	std::unordered_map<uint32_t, MobComponent*> avatars;
};

#endif
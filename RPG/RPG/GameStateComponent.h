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
	GameStateComponent(instream& is);
	~GameStateComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void pre_frame(float dTime);
	void post_frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, const std::shared_ptr<ClientData>& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, const std::shared_ptr<ClientData>& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, const std::shared_ptr<ClientData>& client) const;
	void write_to(outstream& os) const;

	static AutoSerialFactory<GameStateComponent, Component> _factory;

	MobComponent * createAvatar(const std::shared_ptr<ClientData>& client, uint32_t team, uint32_t index);

	void set_display(bool enable);

	std::shared_ptr<std::function<void(RenderSetup&)>> func;

	std::unordered_map<uint32_t, MobComponent*> avatars;
};

#endif
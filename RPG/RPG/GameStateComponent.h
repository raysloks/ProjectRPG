#ifndef GAME_STATE_COMPONENT_H
#define GAME_STATE_COMPONENT_H

#include "Component.h"

#include <memory>
#include <functional>

class RenderSetup;

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

	void set_display(bool enable);

	std::shared_ptr<std::function<void(RenderSetup&)>> func;

	bool game_over;
	std::vector<size_t> scores;
};

#endif
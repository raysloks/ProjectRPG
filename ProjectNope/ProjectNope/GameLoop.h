#ifndef GAME_LOOP_H
#define GAME_LOOP_H

class World;
class Server;
class Client;

class GameLoop
{
public:
	GameLoop(World * world, Server * server, Client * client = 0);
	~GameLoop(void);

	void init(void);
	void tick(void);

	World * world;
	Server * server;
	Client * client;

private:
	double durationInSeconds;
	double fpsCorrection;
	double lag;
};

#endif
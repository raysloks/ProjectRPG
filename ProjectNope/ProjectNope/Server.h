#include "Connection.h"
#include <memory>
#include <boost\asio.hpp>
#include "streams.h"

#ifndef SERVER_H
#define SERVER_H

class World;
class Client;
class ClientData;
class ScriptMemory;
class PNEntity;//could just be Entity or an id
class NewEntity;

class ClientConnection
{
public:
	ClientConnection(const boost::asio::ip::udp::endpoint& ep) : endpoint(ep) {
		silent_time = 0.0f;
	}

	ClientConnection(const boost::asio::ip::udp::endpoint& ep, const std::shared_ptr<ClientData>& dat) : endpoint(ep), data(dat) {
		silent_time = 0.0f;
	}

	std::shared_ptr<ClientData> data;
	boost::asio::ip::udp::endpoint endpoint;
	float silent_time;
};

class Server
{
public:
	Server(World * pWorld);
	virtual ~Server(void);

	virtual void tick(float dTime);

	void NotifyOfCreation(uint32_t id);
	void NotifyOfRemoval(uint32_t id, uint32_t uid);

	void open(unsigned short port);
	void close(void);

	void handle_packet(const std::shared_ptr<Packet>& packet);

	std::shared_ptr<Connection> con;
	std::map<boost::asio::ip::udp::endpoint, std::shared_ptr<ClientConnection>> conns;

	World * world;
	Client * client;

	boost::asio::ip::udp::endpoint ep;

	std::vector<std::function<void(instream&, ClientData&)>> packet_handler;

	int snapshotTimer, snapshotRate;

	bool snapshotBeforeTick;

	std::shared_ptr<ScriptMemory> mem;

	virtual void onClientConnect(ClientData& data);
	virtual void onClientDisconnect(ClientData& data);
};

#endif
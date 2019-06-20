#include "streams.h"
#include "Server.h"
#include "World.h"
#include "ClientData.h"

#include "PositionComponent.h"

#include "Client.h"

#include <fstream>

const unsigned int protocol = 0;

#define MAKE_PACKET std::shared_ptr<Packet> _packet_(new Packet());outstream out(&_packet_->buffer);
#define SEND_PACKET(x) con->SendTo(_packet_, x);

Server::Server(World * pWorld)
{
	world = pWorld;
}

Server::~Server(void)
{
}

void Server::reset(std::function<void(void)> func)
{
	world->clear();
	if (func)
		func();
	onServerActivated();
	if (client)
		if (client->clientData)
			onClientConnect(*client->clientData);
	for each (auto con in conns)
	{
		if (con.second->data)
			onClientConnect(*con.second->data);
	}
}

#include "Script.h"

#include "StringResource.h"

#include "Profiler.h"

#include "Matrix3.h"
#include "GraphicsComponent.h"

void Server::tick(float dTime)
{
	if (mem == nullptr) {
		std::shared_ptr<StringResource> config = Resource::get<StringResource>("server_config.txt");
		if (config != nullptr)
		{
			mem = std::make_shared<ScriptMemory>();
			Script script(std::istringstream(config->string));
			script.run(mem);
		}
	}

	if (con != nullptr)
	{
		for (auto i = conns.begin(); i != conns.end(); ++i)
		{
			auto& conn = i->second;
			conn->silent_time += dTime;
		}

		con->mutex.lock();
		std::queue<std::shared_ptr<Packet>> recv_buf(con->recv_buf);
		while (con->recv_buf.size())
			con->recv_buf.pop();
		con->mutex.unlock();

		while (recv_buf.size())
		{
			handle_packet(recv_buf.front());
			recv_buf.pop();
		}

		std::set<boost::asio::ip::udp::endpoint> removed;
		for (auto i = conns.begin(); i != conns.end(); ++i)
		{
			std::shared_ptr<ClientConnection> conn = i->second;
			if (conn->silent_time > 4.0f) {
				if (conn->data != nullptr)
					onClientDisconnect(*conn->data);
				removed.insert(conn->endpoint);
			}
		}

		for (auto i = removed.begin(); i != removed.end(); ++i)
		{
			std::cout << *i << " timed out." << std::endl;

			conns.erase(*i);
		}
	}

	if (!snapshotBeforeTick)
		if (world->authority)
			world->tick(dTime);

	if (con != nullptr)
	{
		TimeslotA(snapshot);

		--snapshotTimer;

		for (auto i = conns.begin(); i != conns.end(); ++i)
		{
			std::shared_ptr<ClientConnection> conn = i->second;
			if (conn->data != nullptr)
			{
				std::vector<std::pair<size_t, NewEntity*>> added_units;

				for (size_t j = 0; j < world->units.size(); j++)
				{
					if (conn->data->getUnit(j) == 0xffffffff)
					{
						NewEntity * ent = world->units[j];
						if (ent != nullptr)
						{
							added_units.push_back(std::make_pair(conn->data->addKnownUnit(j), ent));
						}
					}
				}

				for (auto added_unit : added_units)
				{
					uint32_t client_side_id = added_unit.first;
					NewEntity * ent = added_unit.second;
					MAKE_PACKET;
					out << (unsigned char)1 << client_side_id << conn->data->unit_uid[client_side_id];
					ent->write_to(out, conn->data);

					for (size_t i = 0; i < ent->ss.sync.size(); i++)
						conn->data->sync[client_side_id].insert(std::make_pair(i, ent->ss.sync[i]));

					SEND_PACKET(conn->endpoint);
				}

				for (size_t j = 0; j < conn->data->known_units.size(); j++)
				{
					uint32_t client_side_id = j;
					uint32_t real_id = conn->data->getRealID(client_side_id);
					if (real_id != 0xffffffff)
					{
						NewEntity * ent = world->units[real_id];
						if (ent != nullptr)
						{
							if (!conn->data->up_to_date[client_side_id])
							{
								MAKE_PACKET;
								out << (unsigned char)1 << client_side_id << conn->data->unit_uid[client_side_id];
								ent->write_to(out, conn->data);
								SEND_PACKET(conn->endpoint);
							}

							//update the relevant sync
							for (auto i = ent->ss.conf.begin(); i != ent->ss.conf.end(); ++i)
							{
								conn->data->sync[client_side_id].insert(*i);
							}
						}
					}
				}
			}
		}

		if (snapshotTimer <= 0)
		{
			int64_t freq, start, end;

			QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
			QueryPerformanceCounter((LARGE_INTEGER*)&start);

			snapshotTimer += snapshotRate;
			for (auto i = conns.begin(); i != conns.end(); ++i)
			{
				std::shared_ptr<ClientConnection> conn = i->second;
				for (size_t j = 0; j < conn->data->known_units.size(); j++)
				{
					uint32_t client_side_id = j;
					uint32_t real_id = conn->data->getRealID(client_side_id);
					if (real_id != 0xffffffff)
					{
						NewEntity * ent = world->GetEntity(real_id);
						if (ent != nullptr)
						{
							auto sync_map = conn->data->sync[client_side_id];
							ent->ss.prep(sync_map, *conn->data);

							std::stringbuf buf;
							outstream temp(&buf);

							ent->writeLog(temp, conn->data);

							if (buf.str().size())
							{

								++conn->data->per_entity_sync[client_side_id];

								MAKE_PACKET;

								out << (unsigned char)2 << client_side_id << conn->data->unit_uid[client_side_id] << conn->data->per_entity_sync[client_side_id];

								//send unconfirmed
								out << (uint32_t)ent->ss.npass;
								for (auto i = sync_map.begin(); i != sync_map.end(); i++)
								{
									if (ent->ss.pass[i->first])
										out << (uint32_t)i->first << i->second;
								}

								out.write(buf.str().data(), buf.str().size());

								SEND_PACKET(conn->endpoint);

							}
						}
					}
				}
			}

			for (size_t j = 0; j < world->units.size(); j++)
			{
				NewEntity * ent = world->units[j];
				if (ent != nullptr)
					ent->ss.conf.clear();
			}

			QueryPerformanceCounter((LARGE_INTEGER*)&end);

			Profiler::set("snapshot", 1.0 / (static_cast<double>(end - start) / freq));
		}
	}

	if (snapshotBeforeTick)
		if (world->authority)
			world->tick(dTime);

	//Profiler::print();
}

void Server::NotifyOfCreation(uint32_t id) //TODO merge duplicate code
{
	for (auto i = conns.begin(); i != conns.end(); ++i)
	{
		std::shared_ptr<ClientConnection> conn = i->second;
		if (conn->data != nullptr)
		{
			NewEntity * ent = world->units[id];
			if (ent != nullptr)
			{
				if (conn->data->getUnit(id) == 0xffffffff)
				{
					uint32_t client_side_id = conn->data->addKnownUnit(id);

					MAKE_PACKET;

					out << (unsigned char)1 << client_side_id << conn->data->unit_uid[client_side_id];
					ent->write_to(out, conn->data);

					for (size_t i = 0; i < ent->ss.sync.size(); ++i)
						conn->data->sync[client_side_id].insert(std::make_pair(i, ent->ss.sync[i]));

					SEND_PACKET(conn->endpoint);
				}
			}
		}
	}
}

void Server::NotifyOfRemoval(uint32_t id)
{
	for (auto i = conns.begin(); i != conns.end(); ++i)
	{
		std::shared_ptr<ClientConnection> conn = i->second;
		uint32_t client_side_id = conn->data->forgetUnit(id);
		if (client_side_id != 0xffffffff)
		{
			MAKE_PACKET;
			out << (unsigned char)3 << client_side_id << conn->data->unit_uid[client_side_id];
			SEND_PACKET(conn->endpoint);
		}
	}

	if (client != nullptr)
	{
		if (client->clientData != nullptr)
			client->clientData->forgetUnit(id);
	}
}

void Server::onServerActivated(void)
{
}

void Server::onClientConnect(ClientData& data)
{
}

void Server::onClientDisconnect(ClientData& data)
{
}

void Server::open(unsigned short port)
{
	ep = boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port);
	con = std::shared_ptr<Connection>(new Connection(ep));

	snapshotTimer = 0;
	snapshotRate = 2;

	snapshotBeforeTick = false;
}

void Server::close(void)
{
	con.reset();
}

void Server::handle_packet(const std::shared_ptr<Packet>& packet)
{
	if (conns.find(packet->endpoint) == conns.end())
	{
		conns.insert(std::pair<boost::asio::ip::udp::endpoint, std::shared_ptr<ClientConnection>>(packet->endpoint, std::shared_ptr<ClientConnection>(new ClientConnection(packet->endpoint))));
		conns[packet->endpoint]->data = std::shared_ptr<ClientData>(new ClientData());
		conns[packet->endpoint]->data->client_id = std::hash<std::string>()(packet->endpoint.address().to_string() + ":" + std::to_string(packet->endpoint.port()));
		onClientConnect(*conns[packet->endpoint]->data);
		std::cout << packet->endpoint << " connected." << std::endl;
	}

	instream in(&packet->buffer);
	in.exceptions(std::ios_base::eofbit);
	std::shared_ptr<ClientConnection> conn = conns[packet->endpoint];
	if (packet->buffer.size())
		conn->silent_time = 0.0f;
	unsigned char type;
	try
	{
		in >> type;
		switch (type)
		{
		case 0:
		{
			uint8_t counter;
			in >> counter;
			MAKE_PACKET;
			out << (uint8_t)0 << counter;
			SEND_PACKET(conn->endpoint);
			break;
		}
		case 1:
		{
			uint32_t id, sync;
			uint32_t size, index;
			in >> id >> size;
			for (size_t i = 0; i < size; ++i)
			{
				in >> index >> sync;
				if (id < conn->data->sync.size())
				{
					if (conn->data->sync[id].find(index) != conn->data->sync[id].end())
						if (conn->data->sync[id][index] == sync)
							conn->data->sync[id].erase(index);
				}
			}
			break;
		}
		case 2:
		{
			uint32_t id;
			uint32_t size;
			in >> size;
			for (size_t i = 0; i < size; ++i)
			{
				in >> id;
				conn->data->forgetUnit(conn->data->getRealID(id));
			}
			break;
		}
		case 3:
		{
			uint32_t id, uid;
			in >> id >> uid;
			if (uid == conn->data->unit_uid[id])
			{
				conn->data->up_to_date[id] = true;
			}
			break;
		}
		case 7:
		{
			uint32_t id;
			in >> id;
			NewEntity * ent = world->GetEntity(conn->data->getRealID(id));
			if (ent != nullptr)
				ent->readLog(in, conn->data);
			break;
		}
		default:
		{
			if (type < packet_handler.size())
			{
				auto func = packet_handler[type];
				if (func != nullptr)
					func(in, *conn->data);
			}
			break;
		}
		}
	}
	catch (std::exception& e)
	{
		//conns[i]->con->Disconnect();
		std::cout << "protocol exception " << e.what() << " in branch " << (int)type << std::endl;
	}
}
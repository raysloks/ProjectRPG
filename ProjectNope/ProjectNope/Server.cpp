#include "streams.h"
#include "Server.h"
#include "World.h"
#include "ClientData.h"

#include "PositionComponent.h"

#include "Chunk.h"

#include "Client.h"
extern Client * client;

#include <fstream>

const unsigned int protocol = 0;

#define MAKE_PACKET std::shared_ptr<Packet> _packet_(new Packet());outstream out(&_packet_->buffer);
#define SEND_PACKET(x) con->SendTo(_packet_, x);

float load_distance_sqr = 500000.0f;
float unload_distance_sqr = 600000.0f;

Server::Server(World * pWorld, unsigned short port)
{
	world = pWorld;

	ep = boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port);
	con = std::shared_ptr<Connection>(new Connection(ep));

	snapshotTimer = 0;
	snapshotRate = 2;

	snapshotBeforeTick = false;

	world->level = "test";
	world->save = "test_save";
}

Server::Server(World * pWorld)
{
	world = pWorld;

	world->level = "test";
	world->save = "test_save";
}

Server::~Server(void)
{
}

#include "Script.h"

#include "StringResource.h"

#include "Profiler.h"

#include "Matrix3.h"
#include "GraphicsComponent.h"

void Server::tick(float dTime)
{
	if (mem==0) {
		std::shared_ptr<StringResource> config = Resource::get<StringResource>("server_config.txt");
		if (config!=0)
		{
			mem.reset(new ScriptMemory());
			Script script(std::istringstream(config->string));
			script.run(mem);
		}
	}

	if (con!=0)
	{
		for (auto i=conns.begin();i!=conns.end();++i)
		{
			std::shared_ptr<ClientConnection> conn = i->second;
			conn->silent_time += dTime;

			if (conn->data->unit_id==-1)
				connectClientToEntity(*conn->data);
		}

		con->mutex.lock();
		std::queue<std::shared_ptr<Packet>> recv_buf(con->recv_buf);
		while(con->recv_buf.size())
			con->recv_buf.pop();
		con->mutex.unlock();

		while(recv_buf.size())
		{
			handle_packet(recv_buf.front());
			recv_buf.pop();
		}

		std::set<boost::asio::ip::udp::endpoint> removed;
		for (auto i=conns.begin();i!=conns.end();++i)
		{
			std::shared_ptr<ClientConnection> conn = i->second;
			if (conn->silent_time>4.0f) {
				if (conn->data!=0)
					onClientDisconnect(*conn->data);
				removed.insert(conn->endpoint);
			}
		}

		for (auto i=removed.begin();i!=removed.end();++i)
		{
			std::cout << *i << " timed out." << std::endl;

			conns.erase(*i);
		}
	}

	if (!snapshotBeforeTick)
		if (world->authority)
			world->tick(dTime);

	if (con!=0)
	{
		--snapshotTimer;

		for (auto i=conns.begin();i!=conns.end();++i) {
			std::shared_ptr<ClientConnection> conn = i->second;
			if (conn->data!=0) {
				for (int j=0;j<world->units.size();++j)
				{
					if (conn->data->getUnit(j)<0)
					{
						NewEntity * ent = world->units[j];
						if (ent!=0)
						{
							if (isVisibleTo(ent, *conn->data))
							{
								int client_side_id = conn->data->addKnownUnit(j);
								MAKE_PACKET
									out << (unsigned char)1 << client_side_id << world->uid[j];
								ent->write_to(out, *conn->data);

								for (int i=0;i<ent->ss.sync.size();++i)
									conn->data->sync[client_side_id].insert(std::make_pair(i, ent->ss.sync[i]));

								SEND_PACKET(conn->endpoint)
							}
						}
					}
				}

				for (int j=0;j<conn->data->known_units.size();++j)
				{
					if (conn->data->known_units[j]>=0)
					{
						NewEntity * ent = world->units[conn->data->known_units[j]];
						if (ent!=0)
						{
							if (!isVisibleTo(ent, *conn->data))
							{
								MAKE_PACKET
									out << (unsigned char)3 << conn->data->forgetUnit(conn->data->known_units[j]) << world->uid[conn->data->known_units[j]];
								SEND_PACKET(conn->endpoint)
							}
							else
							{
								//update the relevant sync
								for (auto i=ent->ss.conf.begin();i!=ent->ss.conf.end();++i)
								{
									conn->data->sync[j].insert(*i);
								}
							}
						}
					}
				}

				if (world->use_chunks)
				{
					if (conn->data->unit_id!=-1)
					{
						NewEntity * unit = world->GetEntity(conn->data->unit_id);
						if (unit!=0)
						{
							PositionComponent * p = unit->getComponent<PositionComponent>();
							if (p!=0) {
								world->LoadSurroundings(unit);

								std::vector<int> pos; //TODO make function for this
								pos.push_back((p->p.x)/(chunk_size));
								pos.push_back((p->p.y)/(chunk_size));
								pos.push_back((p->p.z)/(chunk_size));

								conn->data->loading = true;
								if (world->chunks[pos]!=0)
									conn->data->loading = world->chunks[pos]->loading;
							} //loading doesn't change if player doesn't have a position or chunks are disabled
						}
					}
				}
				else
				{
					conn->data->loading = false;
				}
			}
		}
	
		if (--snapshotTimer <= 0)
		{
			snapshotTimer += snapshotRate;
			for (auto i=conns.begin();i!=conns.end();++i)
			{
				std::shared_ptr<ClientConnection> conn = i->second;
				//if (conn->data->client_id!=-1) {
				/*MAKE_PACKET
					out << (unsigned char)0;
				conn->data->write(out);
				SEND_PACKET(conn->endpoint)*/
					//}
				for (int j=0;j<conn->data->known_units.size();++j)
				{
					if (conn->data->known_units[j]>=0) {
						NewEntity * ent = world->GetEntity(conn->data->known_units[j]);
						if (ent!=0) {
							auto sync_map = conn->data->sync[j];
							ent->ss.prep(sync_map, *conn->data);

							std::stringbuf buf;
							outstream temp(&buf);

							ent->writeLog(temp, *conn->data);

							if (buf.str().size()) {

								SyncState::increment(conn->data->per_entity_sync[j]);

								MAKE_PACKET
									out << (unsigned char)2 << j << world->uid[j] << conn->data->per_entity_sync[j];

								//send unconfirmed
								out << (uint32_t)ent->ss.npass;
								for (auto i=sync_map.begin();i!=sync_map.end();++i)
								{
									if (ent->ss.pass[i->first])
										out << (uint32_t)i->first << i->second;
								}

								out.write(buf.str().data(), buf.str().size());

								SEND_PACKET(conn->endpoint)

							}
						}
					}
				}
			}
			for (int j=0;j<world->units.size();++j)
			{
				NewEntity * ent = world->units[j];
				if (ent!=0)
					ent->ss.conf.clear();
			}
		}
	}

	std::map<std::vector<int>, Chunk*> removed;
	for (std::map<std::vector<int>, Chunk*>::iterator chunk=world->chunks.begin();chunk!=world->chunks.end();++chunk)
	{
		if (chunk->second!=0)
		{
			chunk->second->sync();
			for (auto i=chunk->second->units.begin();i!=chunk->second->units.end();++i)
			{
				NewEntity * ent = world->GetEntity(*i);
				if (ent!=0)//maybe assert here
				{
					PositionComponent * p = ent->getComponent<PositionComponent>();
					if (p!=0) {
						std::vector<int> pos;
						pos.push_back((p->p.x)/(chunk_size));
						pos.push_back((p->p.y)/(chunk_size));
						pos.push_back((p->p.z)/(chunk_size));
						if (pos!=chunk->first)
						{
							if (world->chunks[pos]!=0) {
								chunk->second->transfer(i-chunk->second->units.begin(), world->chunks[pos]);
							}
						}
					}
				}
			}			

			bool in_range = false;
			for (auto i=conns.begin();i!=conns.end();++i) {
				std::shared_ptr<ClientConnection> conn = i->second;
				if (conn->data!=0) {
					if (conn->data->unit_id!=-1)
					{
						NewEntity * ent = world->GetEntity(conn->data->unit_id);
						if (ent!=0)
						{
							PositionComponent * p = ent->getComponent<PositionComponent>();
							if (p!=0) {
								std::vector<int> pos;
								pos.push_back((p->p.x)/(chunk_size));
								pos.push_back((p->p.y)/(chunk_size));
								pos.push_back((p->p.z)/(chunk_size));
								int range = 3;
								if (abs(chunk->second->pos[0]-pos[0])<range)
									if (abs(chunk->second->pos[1]-pos[1])<range)
										if (abs(chunk->second->pos[2]-pos[2])<range)
											in_range = true;
							}
						}
					}
				}
			}

			if (client!=0)
			{
				if (client->clientData!=0) {
					if (client->clientData->unit_id!=-1)
					{
						NewEntity * ent = world->GetEntity(client->clientData->unit_id);
						if (ent!=0)
						{
							PositionComponent * p = ent->getComponent<PositionComponent>();
							if (p!=0) {
								std::vector<int> pos;
								pos.push_back((p->p.x)/(chunk_size));
								pos.push_back((p->p.y)/(chunk_size));
								pos.push_back((p->p.z)/(chunk_size));
								int range = 3;
								if (abs(chunk->second->pos[0]-pos[0])<range)
									if (abs(chunk->second->pos[1]-pos[1])<range)
										if (abs(chunk->second->pos[2]-pos[2])<range)
											in_range = true; //TODO merge duplicate code
							}
						}
					}
				}
			}

			if (!in_range)
				removed.insert(std::make_pair(chunk->first, chunk->second));
		}
	}

	for (auto i=removed.begin();i!=removed.end();++i)
	{
		i->second->unload();
		delete i->second;
		world->chunks.erase(i->first);
	}

	if (snapshotBeforeTick)
		if (world->authority)
			world->tick(dTime);

	//Profiler::print();
}

void Server::NotifyOfCreation(int id) //TODO merge duplicate code
{
	for (auto i=conns.begin();i!=conns.end();++i) {
		std::shared_ptr<ClientConnection> conn = i->second;
		if (conn->data!=0) {
			if (conn->data->unit_id!=-1)
			{
				NewEntity * unit = world->GetEntity(conn->data->unit_id);
				if (unit!=0)
				{
					Vec3 dif;

					NewEntity * ent = world->units[id];
					if (ent!=0)
					{
						if (isVisibleTo(ent, *conn->data))
						{
							if (conn->data->getUnit(id)<0)
							{
								int client_side_id = conn->data->addKnownUnit(id);
								MAKE_PACKET
									out << (unsigned char)1 << client_side_id << world->uid[id];
								ent->write_to(out, *conn->data);

								for (int i=0;i<ent->ss.sync.size();++i)
									conn->data->sync[client_side_id].insert(std::make_pair(i, ent->ss.sync[i]));

								SEND_PACKET(conn->endpoint)
							}
						}
					}
				}
			}
		}
	}
}

void Server::NotifyOfRemoval(int id, int uid)
{
	for (auto i=conns.begin();i!=conns.end();++i)
	{
		std::shared_ptr<ClientConnection> conn = i->second;
		MAKE_PACKET
		out << (unsigned char)3 << conn->data->forgetUnit(id) << uid;
		SEND_PACKET(conn->endpoint)
	}
	if (client!=0)
	{
		if (client->clientData!=0)
			client->clientData->forgetUnit(id);
	}
}

void Server::connectClientToEntity(ClientData& data)
{
	data.unit_id = 0;
	/*if (data.client_type==0) {
		PlayerUnit * player = new PlayerUnit(Vec3(), Vec3(0.0f, 0.0f, 0.0f));
		data.unit_id = world->AddEntity(player);
	}
	if (data.client_type==1)
		data.unit_id = world->AddEntity(new Spectator(Vec3()));*/
}

bool Server::isVisibleTo(NewEntity * ent, ClientData& data)
{
	return true;
	NewEntity * unit = world->GetEntity(data.unit_id);
	if (ent!=0 && unit!=0) {
		PositionComponent * p_ent = ent->getComponent<PositionComponent>();
		PositionComponent * p_unit = unit->getComponent<PositionComponent>();
		if (p_ent!=0 && p_unit!=0) {
			Vec3 dif = p_ent->p-p_unit->p;
			return dif.LenPwr()<load_distance_sqr;//TODO add customizabilityish thingymajig
		}
	}
	return true;
}

void Server::onClientDisconnect(ClientData& data)
{
	if (world->GetEntity(data.unit_id)!=0)
		world->SetEntity(data.unit_id, 0);
}

void Server::handle_packet(const std::shared_ptr<Packet>& packet)
{
	if (conns.find(packet->endpoint)==conns.end())
	{
		conns.insert(std::pair<boost::asio::ip::udp::endpoint, std::shared_ptr<ClientConnection>>(packet->endpoint, std::shared_ptr<ClientConnection>(new ClientConnection(packet->endpoint))));
		conns[packet->endpoint]->data = std::shared_ptr<ClientData>(new ClientData());
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
		switch(type) {
		case 0:
			{
				unsigned int client_protocol;
				in >> client_protocol;
				if (client_protocol==protocol)
				{
					unsigned char client_type;
					in >> client_type;
					if (conn->data->client_type==-1)
						conn->data->client_type = client_type;
				}
				else
				{
					//TODO send wrong protocol version message
				}
				break;
			}
		case 1:
			{
				int id, sync;
				uint32_t size, index;
				in >> id >> size;
				for (auto i=0;i<size;++i)
				{
					in >> index >> sync;
					if (id<conn->data->sync.size())
					{
						if (conn->data->sync[id].find(index)!=conn->data->sync[id].end())
							if (conn->data->sync[id][index]==sync)
								conn->data->sync[id].erase(index);
					}
				}
				break;
			}
		case 2:
			{
				int id;
				uint32_t size;
				in >> size;
				for (auto i=0;i<size;++i)
				{
					in >> id;
					if (id>=0 && id<conn->data->known_units.size())
						conn->data->known_units[id]=-1;
				}
				break;
			}
		case 3:
			{
				NewEntity * ent = world->GetEntity(conn->data->unit_id);
				GlobalPosition pos;
				in >> pos;
				if (ent!=0) {
					PositionComponent * p = ent->getComponent<PositionComponent>();
					if (p!=0) {
						p->p = pos;
					}
				}
				break;
			}
		case 4:
			{
				unsigned char subtype;
				uint32_t l;
				std::set<int> ids;
				in >> subtype >> l;
				int id;
				for (int i=0;i<l;++i) {
					in >> id;
					ids.insert(id);
				}
				switch(subtype)
				{
				case 0:
					{
						Vec3 v;
						in >> v;
						for (auto i=ids.begin();i!=ids.end();++i)
						{
							NewEntity * ent = world->GetEntity(*i);
							if (ent==0)
								break;
							/*ent->set_changed();
							StaticProp * statprop = dynamic_cast<StaticProp*>(ent);
							if (statprop!=0)
								statprop->reconclassing = true;
							if (ent!=0)
								ent->p += v;*/
						}
						break;
					}
				case 1:
					{
						GlobalPosition med;
						Vec3 v;
						in >> med >> v;
						for (auto i=ids.begin();i!=ids.end();++i)
						{
							NewEntity * ent = world->GetEntity(*i);
							if (ent==0)
								break;
							/*ent->set_changed();
							StaticProp * statprop = dynamic_cast<StaticProp*>(ent);
							if (statprop!=0)
								statprop->reconclassing = true;
							if (statprop!=0) {
								float l = v.Len();
								statprop->_mtrx = statprop->_mtrx*Matrix3(l, v/l);
								Vec3 dif = statprop->p-med;
								statprop->p += dif*Matrix3(l, v/l)-dif;
							}*/
							break;
						}
					}
				case 2:
					{
						Vec3 v;
						in >> v;
						for (auto i=ids.begin();i!=ids.end();++i)
						{
							NewEntity * ent = world->GetEntity(*i);
							if (ent==0)
								break;
							/*ent->set_changed();
							StaticProp * statprop = dynamic_cast<StaticProp*>(ent);
							if (statprop!=0)
								statprop->reconclassing = true;
							if (statprop!=0) {
								Matrix3 m;
								m.data[0] = v.x;
								m.data[4] = v.y;
								m.data[8] = v.z;
								statprop->_mtrx = statprop->_mtrx*m;
							}*/
						}
						break;
					}
				case 3:
					{
						for (auto i=ids.begin();i!=ids.end();++i)
						{
							NewEntity * ent = world->GetEntity(*i);
							if (ent==0)
								break;
							/*ent->set_changed();
							StaticProp * statprop = dynamic_cast<StaticProp*>(ent);
							if (statprop!=0)
								statprop->reconclassing = true;
							if (statprop!=0) {
								statprop->_mtrx = Matrix3();
							}*/
						}
						break;
					}
				case 4:
					{
						GlobalPosition p, med;
						in >> p;
						med = world->getMed(ids);
						for (auto i=ids.begin();i!=ids.end();++i)
						{
							NewEntity * ent = world->GetEntity(*i);
							if (ent==0)
								break;
							/*ent->set_changed();
							StaticProp * statprop = dynamic_cast<StaticProp*>(ent);
							if (statprop!=0)
								statprop->reconclassing = true;
							if (ent!=0)
								ent->p += p-med;*/
						}
						break;
					}
				}

				/*if (statprop!=0) {
				for (int i=0;i<3;++i) {
				for (int j=0;j<3;++j)
				std::cout << statprop->_mtrx.mtrx[i][j] << " ";
				std::cout << std::endl;
				}
				}*/
				break;
			}
		case 5:
			{
				NewEntity * ent = new NewEntity(in, true);
				if (ent!=0) {
					int id = world->AddEntity(ent);
					PositionComponent * p = ent->getComponent<PositionComponent>();
					if (p!=0) {
						std::vector<int> pos;
						pos.push_back((p->p.x)/(chunk_size));
						pos.push_back((p->p.y)/(chunk_size));
						pos.push_back((p->p.z)/(chunk_size));
						if (world->chunks[pos]==0)
							world->LoadChunk(pos);
						world->chunks[pos]->add(id);
					}
				}
				break;
			}
		case 6:
			{
				int id;
				in >> id;
				if (id < 0 || id>=conn->data->known_units.size())
					break;
				id = conn->data->known_units[id];
				for (auto i=conns.begin();i!=conns.end();++i)
				{
					if (i->second->data!=0) {
						if (i->second->data->unit_id==id)
							id=-1;
					}
				}
				if (id>=0) {
					world->SetEntity(id, 0);
					std::cout << "deleted entity " << id << std::endl;
				}
				break;
			}
		case 7:
			{
				int id;
				in >> id;
				NewEntity * ent = world->GetEntity(id);
				if (ent!=0)
					ent->readLog(in, *conn->data);
				break;
			}
		case 8:
			{
				std::cout << "saving..." << std::endl;
				for (std::map<std::vector<int>, Chunk*>::iterator chunk=world->chunks.begin();chunk!=world->chunks.end();++chunk)
				{
					if (chunk->second!=0)
					{
						chunk->second->save(std::string("data/level/") + world->level + "/");
					}
				}
				std::cout << "saved." << std::endl;
				break;
			}
		default:
			{
				if (type<packet_handler.size())
				{
					auto func = packet_handler[type];
					if (func!=0)
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
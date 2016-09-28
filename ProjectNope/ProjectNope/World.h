#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <map>
#include <set>
#include <stack>
#include <memory>
#include <boost\thread.hpp>
#include "Serializable.h"//for SerialID
#include "GlobalPosition.h"
#include "Quaternion.h"

class TypeIteratorBase;
class ScriptMemory;
class Matrix3;
class Chunk;
class Collision;
class NewEntity;
class RenderSetup;
class Server;
class Client;

class World
{
public:
	World(void);
	~World(void);

	void frame(float dTime);
	void tick(float dTime);
	void render(RenderSetup& rs);

	void clean(void);
	void clear(void);

	void LoadChunk(const std::vector<int>& pos);
	void LoadSurroundings(NewEntity * entity);

	NewEntity * NearestPlayer(const GlobalPosition& p);

	void UnloadChunk(const std::vector<int>& pos);

	int AddEntity(NewEntity * entity); // use this one over SetEntity most of the time
	
	void SetEntity(int id, NewEntity * entity);
	NewEntity * GetEntity(int id);
	NewEntity * GetEntity(int id, int uid);

	Server * server;
	Client * client;

	std::vector<NewEntity*> units;
	std::vector<int> uid;
	std::vector<NewEntity*> removed;
	std::stack<size_t> alloc;

	std::map<std::pair<int, SerialID>, std::shared_ptr<TypeIteratorBase>> comp_layers;

	GlobalPosition getMed(const std::set<int>& ents);

	void SphereCheck(const GlobalPosition& vP, float r, std::vector<std::shared_ptr<Collision>>& list);
	void LineCheck(const GlobalPosition& sP, const GlobalPosition& eP, std::vector<std::shared_ptr<Collision>>& list);
	void SphereCast(const GlobalPosition& sP, const GlobalPosition& eP, float r, std::vector<std::shared_ptr<Collision>>& list);
	
	void UnitCheck(const GlobalPosition& vP, float r, std::vector<std::shared_ptr<Collision>>& list);
	void UnitCheck(const GlobalPosition& vP, const Matrix3& mtrx, std::vector<std::shared_ptr<Collision>>& list);

	GlobalPosition cam_pos;
	Quaternion cam_rot;

	bool use_chunks;
	std::map<std::vector<int>, Chunk*> chunks;

	std::string level, save;

	std::shared_ptr<ScriptMemory> mem;
		
	bool authority;
	boost::mutex auth_break;
};

#endif
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
#include "PositionComponent.h"

#include "Profiler.h"

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

	void pre_frame(float dTime);
	void post_frame(float dTime);
	void tick(float dTime);
	void render(RenderSetup& rs);

	void clean(void);
	void clear(void);

	int AddEntity(NewEntity * entity); // use this one over SetEntity most of the time
	
	void SetEntity(int id, NewEntity * entity);
	NewEntity * GetEntity(int id);
	NewEntity * GetEntity(int id, int uid);

	std::multimap<float, NewEntity*> GetNearestEntities(const GlobalPosition& p);
	std::multimap<float, NewEntity*> GetNearestEntities(const GlobalPosition& p, float r);

	template <class T>
	std::multimap<float, T*> GetNearestComponents(const GlobalPosition& p)
	{
		Timeslot timeslot_get_nearest("get_nearest");

		std::multimap<float, T*> ret;

		for (auto i = units.begin(); i != units.end(); ++i)
		{
			if (*i != nullptr)
			{
				auto pc = (*i)->getComponent<PositionComponent>();
				if (pc != nullptr)
				{
					float distance = Vec3(p - pc->p).Len();
					auto t = (*i)->getComponent<T>();
					if (t != nullptr)
					{
						ret.insert(std::make_pair(distance, t));
					}
				}
			}
		}

		return ret;
	}

	template <class T>
	std::multimap<float, T*> GetNearestComponents(const GlobalPosition& p, float r)
	{
		Timeslot timeslot_get_nearest("get_nearest");

		std::multimap<float, T*> ret;

		for (auto i = units.begin(); i != units.end(); ++i)
		{
			if (*i != nullptr)
			{
				auto pc = (*i)->getComponent<PositionComponent>();
				if (pc != nullptr)
				{
					float distance = Vec3(p - pc->p).Len();
					if (distance <= r)
					{
						auto t = (*i)->getComponent<T>();
						if (t != nullptr)
						{
							ret.insert(std::make_pair(distance, t));
						}
					}
				}
			}
		}

		return ret;
	}

	Server * server;
	Client * client;

	std::vector<NewEntity*> units;
	std::vector<int> uid;
	std::vector<NewEntity*> removed;
	std::stack<size_t> alloc;

	std::map<std::pair<int, uint32_t>, std::shared_ptr<TypeIteratorBase>> comp_layers;

	GlobalPosition getMed(const std::set<int>& ents);

	void SphereCheck(const GlobalPosition& vP, float r, std::vector<std::shared_ptr<Collision>>& list);
	void LineCheck(const GlobalPosition& sP, const GlobalPosition& eP, std::vector<std::shared_ptr<Collision>>& list);
	void SphereCast(const GlobalPosition& sP, const GlobalPosition& eP, float r, std::vector<std::shared_ptr<Collision>>& list);
	
	void UnitCheck(const GlobalPosition& vP, float r, std::vector<std::shared_ptr<Collision>>& list);
	void UnitCheck(const GlobalPosition& vP, const Matrix3& mtrx, std::vector<std::shared_ptr<Collision>>& list);

	GlobalPosition cam_pos;
	Quaternion cam_rot;

	std::shared_ptr<ScriptMemory> mem;
		
	bool authority;
	boost::mutex auth_break;
};

#endif
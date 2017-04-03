#ifndef AI_COMPONENT_H
#define AI_COMPONENT_H

#include "Component.h"

#include <random>
#include <map>
#include <functional>

#include "Vec3.h"

class MobComponent;

class AIComponent :
	public Component
{
public:
	AIComponent(void);
	AIComponent(instream& is, bool full);
	~AIComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void pre_frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, ClientData& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	static const AutoSerialFactory<AIComponent> _factory;

	MobComponent * mob;

	std::multimap<float, std::function<void(float)>> checks;
	std::function<void(float)> idle, fall;

	std::default_random_engine random;
};

#endif
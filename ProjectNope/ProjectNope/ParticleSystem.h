#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "PNEntity.h"

struct Particle
{
	Particle(void):e(true){}

	Vec3 p, v;
	float t;
	bool e;
};

class ParticleSystem :
	public PNEntity
{
public:
	ParticleSystem(const GlobalPosition& pos);
	ParticleSystem(instream& is, bool full);
	~ParticleSystem(void);

	void writeLog(outstream& os);
	void readLog(instream& is);

	static const AutoSerialFactory<ParticleSystem> _factory_ParticleSystem;

	void remove(size_t index);
	size_t add(void);

	void tick(float dTime);
	void render(const GlobalPosition& origin);

	void interpolate(Entity * pEntity, float fWeight);

	void write_to(outstream& os, bool full) const;

	std::vector<Particle> sys;
	std::stack<size_t> alloc;

	unsigned int seed, dir;
	float t;
};

#endif
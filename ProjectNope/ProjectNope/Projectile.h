#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "PNEntity.h"

class DamageSource;

class Projectile :
	public PNEntity
{
public:
	Projectile(const GlobalPosition& pos, const Vec3& vel);
	Projectile(instream& is, bool full);
	~Projectile(void);

	static const AutoSerialFactory<Projectile> _factory_Projectile;

	virtual void tick(float dTime);
	virtual void render(const GlobalPosition& origin);

	virtual void write_to(outstream& os, bool full) const;

	std::shared_ptr<DamageSource> src;
	Vec3 v;
	float t;
};

#endif
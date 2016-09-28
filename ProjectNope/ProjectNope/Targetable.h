#include "PNEntity.h"

#ifndef TARGETABLE_H
#define TARGETABLE_H

class Collision;

class Targetable :
	public PNEntity
{
public:
	Targetable(const GlobalPosition& pos, const Vec3& vel, float size);
	Targetable(instream& is, bool full);
	~Targetable(void);

	static const AutoSerialFactory<Targetable> _factory_Targetable;

	virtual void tick(float dTime);
	virtual void render(const GlobalPosition& origin);

	virtual void interpolate(Entity * pEntity, float fWeight);

	virtual void write_to(outstream& os, bool full) const;

	Vec3 v;
	float r;

	Vec3 move;

	virtual void OnCollision(const std::shared_ptr<Collision>& col, float dTime);
};

#endif
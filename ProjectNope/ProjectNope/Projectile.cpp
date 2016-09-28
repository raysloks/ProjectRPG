#include "Projectile.h"
#include "Unit.h"
#include "TestServer.h"
#include "World.h"

extern TestServer * server;

const AutoSerialFactory<Projectile> Projectile::_factory_Projectile("Projectile");

Projectile::Projectile(const GlobalPosition& pos, const Vec3& vel) : PNEntity(pos), Serializable(_factory_Projectile.id)
{
	v = vel;
}

Projectile::Projectile(instream& is, bool full) : PNEntity(is, full), Serializable(_factory_Projectile.id)
{
	is >> v;
}

Projectile::~Projectile(void)
{
}

void Projectile::tick(float dTime)
{
	if (world->authority)
	{
		v.z -= 9.8f;
		p += v*dTime;
		t += dTime;
		/*if (t>120.0f)
			world->SetEntity(id, 0);*/
	}
}

void Projectile::render(const GlobalPosition& origin)
{
}

void Projectile::write_to(outstream& os, bool full) const
{
	Entity::write_to(os, full);
	os << v;
}
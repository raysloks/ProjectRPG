#include "DamageSource.h"

DamageSource::DamageSource(Entity * src, int dmg, const Vec3& kb) : source(src), damage(dmg), knockback(kb)
{
}

DamageSource::~DamageSource(void)
{
}
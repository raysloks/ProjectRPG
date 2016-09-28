#include "TargetLockCap.h"
#include "Unit.h"

#include "World.h"

TargetLockCap::TargetLockCap(Unit * u) : UnitCapability(u)
{
}

TargetLockCap::~TargetLockCap(void)
{
}

void TargetLockCap::tick(float dTime)
{
	if (unit->cs.input["lock"]>0.5f)
	{
		if (target_id.id>=0)
		{
			Unit * target = 0;//dynamic_cast<Unit*>(world->GetEntity(target_id));
			if (target!=0)
			{
				float l = Vec3(target->p-unit->p).Len();
				if (l>15.0f || target->dead)
				{
					target_id.id = -1;
				}
			}
			else
			{
				target_id.id = -1;
			}
		}
		else
		{
			std::shared_ptr<Collision> closest;
			float l = 0.0f;
			std::vector<std::shared_ptr<Collision>> list;
			world->UnitCheck(unit->p, 15.0f, list);
			for (auto i=list.begin();i!=list.end();++i)
			{
				if ((*i)->entity!=unit) {
					Unit * other = dynamic_cast<Unit*>((*i)->entity);
					if (other!=0)
					{
						Vec3 dif = Vec3(other->p-unit->p);
						float nl = dif.Len();
						dif /= nl;
						nl = sin(acos(dif.Dot(Vec2(-sin(unit->facing.x), cos(unit->facing.x)))))*nl;
						if ((nl<l || closest==0) && !other->dead)
						{
							closest = *i;
							l=nl;
						}
					}
				}
			}
			if (closest!=0)
				target_id = closest->entity->id;
		}
	}
	else
	{
		target_id.id = -1;
	}

	unit->look_target = target_id;
}
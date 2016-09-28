#include "GruntAI.h"
#include "Unit.h"
#include "PlayerUnit.h"
#include "GruntUnit.h"

#include "World.h"

GruntAI::GruntAI(Unit * u, bool full) : UnitCapability(u), move_it(full)
{
}

GruntAI::~GruntAI(void)
{
}

void GruntAI::tick(float dTime)
{
	if (target_id.id>=0)
	{
		Unit * target = 0;//dynamic_cast<Unit*>(world->GetEntity(target_id));
		if (target!=0)
		{
			Vec3 dif(target->p-unit->p);
			float l = dif.Len();
			Vec3 ndif = dif;
			if (l!=0.0f)
				ndif /= l;
			if (l>15.0f || target->dead)
			{
				target_id.id = -1;
			}
			else
			{
				if (move_it)
				{
					float kl = l;
					Vec2 fd(-sin(unit->facing.x), cos(unit->facing.x));
					Vec2 fdo(-sin(target->facing.x), cos(target->facing.x));

					if (l<1.0f*unit->scale)
					{
						//float r = ((float)rand())/RAND_MAX;
						//if (r>exp(log(0.5f)*dTime))
						if (fd.Dot(ndif)>0.5f)
							unit->cs.activate("attack");
					}

					if (fd.Dot(ndif)<0.0f)
						kl -= 2.0f;
					kl -= 0.5f*unit->scale;
					//kl *= kl;
					if (abs(kl)<0.1f)
						kl = 0.0f;
					dif *= std::min(std::max(kl, -1.0f), 1.0f);

					fm -= ndif;
					fm *= exp(log(0.0f)*dTime);
					fm += ndif;
				}
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
		world->UnitCheck(unit->p, 10.0f, list);
		for (auto i=list.begin();i!=list.end();++i)
		{
			if ((*i)->entity!=unit) {
				{
					PlayerUnit * other = dynamic_cast<PlayerUnit*>((*i)->entity);
					if (other!=0)
					{
						if (!other->dead)
						{
							float nl = Vec3(other->p-unit->p).LenPwr();
							if (nl<l || closest==0)
							{
								closest = *i;
								l=nl;
							}
						}
					}
				}
				{
					GruntUnit * other = dynamic_cast<GruntUnit*>((*i)->entity);
					if (other!=0)
					{
						Vec3 dif = other->p-unit->p;
						float l = dif.Len();
						dif /= l;
						if (l<10.0f)
							move -= dif*dTime/(l+1.0f);
					}
				}
			}
		}
		if (closest!=0)
			target_id = closest->entity->id;

		if (move_it)
		{
			//wander
			float dir = ((float)rand())/RAND_MAX*M_PI*2.0f;
			move += Vec2(cos(dir), sin(dir))*dTime*2.0f;
			float l = move.Len();
			if (l<0.4f)
				move *= 0.4f/l;
			if (l>1.0f)
				move /= l;

			fm -= move;
			fm *= exp(log(0.5f)*dTime);
			fm += move;
		}
	}

	if (move_it)
	{
		float l = fm.Len();
		/*if (l<0.8f && l!=0.0f)
		fm *= 0.8f/l;*/
		if (l>1.0f)
			fm /= l;

		unit->cs.input["x"] = fm.x;
		unit->cs.input["y"] = fm.y;
	}

	unit->look_target = target_id;

	if (unit->dead)
		unit->look_target.id = -1;
}
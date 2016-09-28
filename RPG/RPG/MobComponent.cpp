#include "MobComponent.h"

#include "NewEntity.h"

#include "World.h"

#include "PositionComponent.h"
#include "CameraControlComponent.h"
#include "ColliderComponent.h"

#include "GlobalPosition.h"

#include "ClientData.h"

#include "Client.h"

#include "Input.h"

#include "BlendUtility.h"

const AutoSerialFactory<MobComponent> MobComponent::_factory("MobComponent");

MobComponent::MobComponent(void) : Serializable(_factory.id)
{
	facing = Vec3(0.0f, 1.0f, 0.0f);
	move_facing = facing;
	follow = facing;
	up = Vec3(0.0f, 0.0f, 1.0f);
}

MobComponent::MobComponent(instream& is, bool full) : Serializable(_factory.id)
{
	is >> facing >> move_facing >> up;
}

MobComponent::~MobComponent(void)
{
}

void MobComponent::connect(NewEntity * pEntity, bool authority)
{
}

void MobComponent::disconnect(void)
{
}

#include "Profiler.h"

void MobComponent::tick(float dTime)
{
	if (entity->world->authority)
	{
		for (auto i = input.begin(); i != input.end();)
		{
			i->second -= dTime;
			if (i->second <= 0.0f)
				i = input.erase(i);
			else
				++i;
		}

		auto pc = entity->getComponent<PositionComponent>();
		if (pc!=0)
			p = &pc->p;

		if (move.Len()>1.0f)
			move.Normalize();

		if (p!=0)
		{
			GlobalPosition prev = *p;

			if (input.find("warp")!=input.end())
				*p = GlobalPosition();

			landed = false;
			land_n = Vec3(); // smooth these out for some cases
			land_v = Vec3(); // -''-

			Vec3 g = -Vec3(0.0f, 0.0f, 1.0f) * 9.8f;

			Vec3 dp = (g/2.0f * dTime + v)*dTime;

			float t = 1.0f;

			std::set<void*> ignored;
			std::set<Vec3> previous;

			while (t>0.0f)
			{
				std::vector<std::shared_ptr<Collision>> list;
				ColliderComponent::SphereCast(*p, *p+dp, 0.5f, list);

				std::shared_ptr<Collision> col;
				for (auto i=list.begin();i!=list.end();++i)
				{
					if (ignored.find((*i)->ce)==ignored.end())
						if ((*i)->t>=0.0f && (*i)->t<=1.0f)
							if ((*i)->n.Dot(dp-(*i)->v*dTime)<0.0f)
							{
								if (col!=0)
								{
									if (col->t>(*i)->t)
										col = *i;
								}
								else
								{
									col = *i;
								}
							}
				}

				if (col!=0)
				{
					t -= col->t;

					v += g*dTime*list.front()->t;

					*p = col->poo;

					if (col->n.Dot(g)<-5.0f)
						landed = true;
					if (col->n.Dot(up) > land_n.Dot(up)) {
						land_n = col->n;
						land_v = col->v;
					}

					v -= col->v;
					v -= col->n*col->n.Dot(v);
					v += col->v;

					dp = v * dTime;

					bool should_break = false;
					for (auto i=previous.begin();i!=previous.end();++i)
						if (i->Dot(dp)<=0.0f)
							should_break = true;

					if (should_break)
						break;

					ignored.insert(col->ce);
				}
				else
				{
					*p += dp;
					v += g*dTime;
					break;
				}
			}
			if (landed)
			{
				//up = land_n;

				float speed = 5.0f;
				speed += run && input.find("run_delay")==input.end() ? 4.0f : 0.0f * std::max(0.0f, move.Dot(move_facing));
				speed -= action != 0 ? 3.0f : 0.0f;

				Vec3 target = move * speed;
				target -= land_n*land_n.Dot(target);

				v -= target;

				float c = v.Len();
				if (c>0.0f)
				{
					v.Normalize();

					float a = -0.5f;
					float b = -40.0f;

					c += b/a;

					c *= exp(a*dTime);

					c -= b/a;

					if (c<0.0f)
						c=0.0;

					v *= c;
				}

				v += target;

				if (input.find("jump") != input.end()) {
					v -= land_v;
					if (up.Dot(v)<0.0f)
						v -= up * up.Dot(v);
					v += land_v;
					v += up * 4.0f;
					input.erase("jump");
				}

				if (input.find("dodge") != input.end() && dodge <= 0.0f) {
					dodge = 0.3f;
					input.erase("dodge");
				}
			}

			if (dodge > 0.0f) {
				dodge -= dTime;
				Vec3 target = move * (50.0f * dodge + 5.0f);
				target -= land_n*land_n.Dot(target);
				v = target;
			}

			if (prev != *p)
				if (pc != 0)
					pc->update();
		}

		bool use_cam = !run && strafe;

		Vec3 right = cam_facing.Cross(up);

		Vec3 flat_target = move - up*up.Dot(move);
		if (use_cam)
		{
			if (flat_target != Vec3())
			{
				float fty = flat_target.Dot(cam_facing);
				flat_target += cam_facing * 0.5f;
				flat_target -= right*flat_target.Dot(right) * (1.0f - flat_target.Dot(cam_facing));
				flat_target -= cam_facing * std::min(0.0f, flat_target.Dot(cam_facing))*2.0f;
				flat_target += cam_facing * std::min(0.5f, std::max(0.0f, fty + 0.5f));
			}
			if (action)
				flat_target = cam_facing;
		}
		Vec3 flat_facing = facing-up*up.Dot(facing);
		Vec3 flat_move_facing = move_facing-up*up.Dot(move_facing);

		flat_target.Normalize();
		flat_facing.Normalize();
		flat_move_facing.Normalize();

		if (flat_target != Vec3())
			follow = bu_sphere(follow, flat_target, up, log(0.002f), -6.0f, dTime);

		move_facing = bu_sphere(flat_move_facing, follow, up, log(0.002f), -6.0f, dTime);
		if (use_cam)
		{
			facing = bu_sphere(facing, cam_facing, up, log(0.1f), -std::max(std::min(time_under_control + dTime*dTime*0.5f, 0.4f)*50.0f, 6.0f), dTime);
			time_under_control += dTime;
		}
		else
		{
			facing = bu_sphere(flat_facing, follow, up, log(0.002f), -6.0f, dTime);
			time_under_control = 0.0f;
		}

		facing.Normalize();
	}
}

void MobComponent::writeLog(outstream& os, ClientData& client)
{
	os << facing << move_facing << up;
	os << v << land_n << land_v << landed;
}

void MobComponent::readLog(instream& is)
{
	is >> facing >> move_facing >> up;
	is >> v >> land_n >> land_v >> landed;
}

void MobComponent::writeLog(outstream& os)
{
}

void MobComponent::readLog(instream& is, ClientData& client)
{
}

void MobComponent::interpolate(Component * pComponent, float fWeight)
{
	auto mob = dynamic_cast<MobComponent*>(pComponent);
	if (mob!=0)
	{
		facing = bu_sphere(mob->facing, facing, up, fWeight);
		move_facing = bu_sphere(mob->move_facing, move_facing, up, fWeight);
		up = bu_sphere(mob->up, up, facing.Cross(up), fWeight);
		v = mob->v;// v*(1.0f - fWeight) + mob->v*fWeight;
		land_n = mob->land_n;
		land_v = mob->land_v;
		landed = mob->landed;
	}
}

void MobComponent::write_to(outstream& os, ClientData& client) const
{
	os << facing << move_facing << up;
}

void MobComponent::write_to(outstream& os) const
{
}
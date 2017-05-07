#include "MobComponent.h"

#include "NewEntity.h"

#include "World.h"

#include "PositionComponent.h"
#include "CameraControlComponent.h"
#include "ColliderComponent.h"
#include "GraphicsComponent.h"
#include "ProjectileComponent.h"
#include "WeaponComponent.h"
#include "PoseComponent.h"
#include "AnimationControlComponent.h"
#include "AIComponent.h"

#include "GlobalPosition.h"

#include "ClientData.h"

#include "Client.h"

#include "Input.h"

#include "BlendUtility.h"

const AutoSerialFactory<MobComponent> MobComponent::_factory("MobComponent");

MobComponent::MobComponent(void) : Serializable(_factory.id), health(100), stamina(100)
{
	facing = Vec3(0.0f, 1.0f, 0.0f);
	move_facing = facing;
	follow = facing;
	up = Vec3(0.0f, 0.0f, 1.0f);
}

MobComponent::MobComponent(instream& is, bool full) : Serializable(_factory.id)
{
	is >> facing >> move_facing >> cam_facing >> up;
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

#include "LineComponent.h"

void MobComponent::tick(float dTime)
{
	if (entity->world->authority)
	{
		if (health.current <= 0)
		{
			auto acc = entity->getComponent<AnimationControlComponent>();
			if (acc)
			{
				acc->set_state(1);
			}
			auto ai = entity->getComponent<AIComponent>();
			if (ai)
			{
				ai->checks.clear();
			}

			move = Vec3();
			crouch = true;
			health.current -= dTime * (temp_team == 1 ? 100.0f : 1.0f);
			if (health.current <= -300.0f)
				entity->world->SetEntity(entity->id, nullptr);
		}

		for (auto i = input.begin(); i != input.end();)
		{
			i->second -= dTime;
			if (i->second <= 0.0f)
				i = input.erase(i);
			else
				++i;
		}

		auto pc = entity->getComponent<PositionComponent>();
		if (pc != nullptr)
		{
			p = &pc->p;
		}

		if (move.Len()>1.0f)
			move.Normalize();

		if (p != nullptr)
		{
			auto mobs = entity->world->GetNearestComponents<MobComponent>(*p, 2.0f);
			for each (auto mob in mobs)
			{
				if (mob.first < 1.0f && mob.second != this)
				{
					Vec3 dif = *p - *mob.second->p;
					float l = dif.Len();
					dif /= l;

					Vec3 vdif = v - mob.second->v;
					vdif = dif * dif.Dot(vdif);

					v -= vdif * 0.5f;
					mob.second->v += vdif * 0.5f;

					dif *= 1.0f - l;
					dp += dif * 0.5f;
					mob.second->dp -= dif * 0.5f;
				}
			}

			if (input.find("attack") != input.end())
			{
				attack();
			}

			/*{
				std::vector<std::shared_ptr<Collision>> list;
				ColliderComponent::DiskCast(*p + Vec3(0.0f, 0.0f, -0.9f), *p + Vec3(0.0f, 0.0f, -1.1f), 0.25f, list);

				std::shared_ptr<Collision> col;
				for (auto i = list.begin(); i != list.end(); ++i)
				{
					if ((*i)->t >= 0.0f && (*i)->t <= 1.0f)
						if (col != 0)
						{
							if (col->t>(*i)->t)
								col = *i;
						}
						else
						{
							col = *i;
						}
				}

				if (col != 0)
				{
					*p = col->poo;
					v -= col->n*col->n.Dot(v);
				}
			}*/

			GlobalPosition prev = *p;

			//v += move * dTime * 100.0f;

			landed = false;
			land_n = Vec3(); // smooth these out for some cases
			land_v = Vec3(); // -''-

			Vec3 g_dir = Vec3(0.0f, 0.0f, -1.0f);//-Vec3(*p).Normalized();
			Vec3 g = g_dir * 9.8f;

			dp += (g/2.0f * dTime + v) * dTime;

			float t = 1.0f;

			std::set<void*> ignored;
			std::set<Vec3> previous;

			/*auto wpc = weapon->entity->getComponent<PositionComponent>();
			if (wpc != nullptr)
			{
				wpc->p = *p;
				wpc->update();
			}*/

			while (t>0.0f)
			{
				std::vector<std::shared_ptr<Collision>> list;

				if (t == 1.0f)
				{
					auto line = entity->getComponent<LineComponent>();
					if (line != 0)
					{
						line->lines.clear();
						line->lines.resize(16);
						line->color.clear();
						line->color.resize(16);
					}

					/*ColliderComponent::DiskCast(*p, *p - up * (0.25f), 0.5f, list);
					if (!list.empty())
					{
						std::sort(list.begin(), list.end(), [](const std::shared_ptr<Collision>& a, const std::shared_ptr<Collision>& b) { return a->t < b->t; });
						if (list.front()->n.Dot(up) > 0.5f)
						{
							v += up * dTime * 25.0f;
							landed = true;
						}
					}
					list.clear();*/

					float disk_radius = 0.25f;
					float offset = 0.0f;
					float height = crouch ? 1.0f : 1.5f;
					float standing_height = crouch ? 0.75f : 1.25f;
					ColliderComponent::DiskCast(*p - up * offset, *p - up * (offset + height), disk_radius, list);
					if (!list.empty())
					{
						list.erase(std::remove_if(list.begin(), list.end(), [](const std::shared_ptr<Collision>& col)
						{
							return col->n.z <= 0.5f;
						}), list.end());
						if (!list.empty())
						{
							std::sort(list.begin(), list.end(), [](const std::shared_ptr<Collision>& a, const std::shared_ptr<Collision>& b) { return a->t < b->t; });
							Vec3 axis = Vec3(list.front()->poc - list.front()->poo).Cross(up).Normalize();
							GlobalPosition pivot = list.front()->poc;
							GlobalPosition center = list.front()->poo;

							/*auto wpc = weapon->entity->getComponent<PositionComponent>();
							if (wpc != nullptr)
							{
							wpc->p = list.front()->poc;
							wpc->update();
							}*/

							Vec3 n = list.front()->n;
							float tdt = list.front()->t;
							list.clear();

							if (line != 0)
							{
								line->lines[0].first = pivot - *p;
								line->lines[0].second = pivot + up - *p;
								line->color[0].first = Vec3(1.0f, 1.0f, 1.0f);
								line->color[0].second = Vec3(1.0f, 0.0f, 0.0f);

								line->lines[3].first = pivot - *p;
								line->lines[3].second = center - *p;
								line->color[3].first = Vec3(1.0f, 0.0f, 0.0f);
								line->color[3].second = Vec3(1.0f, 0.0f, 0.0f);

								line->lines[6].first = pivot - *p;
								line->lines[6].second = pivot + axis - *p;
								line->color[6].first = Vec3(1.0f, 0.0f, 0.0f);
								line->color[6].second = Vec3(0.0f, 0.0f, 0.0f);
							}

							if (pivot == center)
							{
								std::shared_ptr<Collision> col(new Collision());
								col->t = 0.0f;
								col->n = n;
								col->poo = center + up * standing_height;
								list.push_back(col);
							}
							else
							{
								std::shared_ptr<Collision> col(new Collision());
								col->t = 0.0f;
								col->n = n;
								col->poo = center + up * standing_height;
								list.push_back(col);
							}
						}
					}

					if (!list.empty())
					{
						if (list.front()->n.Dot(up) > 0.5f)
						{
							landed = true;
						}
						land_n = list.front()->n;
						land_v = list.front()->v;

						if (up.Dot(list.front()->poo - *p) < 0.0f)
						{
							list.clear();
						}
					}
				}

				/*for (auto i = list.begin(); i != list.end(); ++i)
				{
					(*i)->poo += up * 0.75f;
					(*i)->n = up;
				}*/

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

					if (col->n.Dot(up)>0.5f)
						landed = true;
					if (col->n.Dot(up) > land_n.Dot(up)) {
						land_n = col->n;
						land_v = col->v;
					}

					v -= col->v;

					if (temp_team == 0)
					{
						float fall_damage = std::fmaxf(0.0f, -10.0f - col->n.Dot(v));
						fall_damage *= 40.0f;
						fall_damage = std::fminf(200.0f, fall_damage);
						health.current -= fall_damage;
						if (fall_damage > 0.0f)
							hit = true;
					}

					v -= col->n*col->n.Dot(v);
					v += col->v;

					dp -= col->n*col->n.Dot(dp);

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
					v += g*dTime*t;
					break;
				}
			}
			
			dp = Vec3();

			if (crouch)
				run = false;

			if (landed)
			{
				float speed = crouch ? 0.0f : run ? 7.0f : 3.5f;
				if (!run)
					speed *= stamina.current / 100.0f;
				speed += 2.0f;

				Vec3 target = move;
				Vec3 target_right = target.Cross(up);
				target = land_n.Cross(target_right);
				target.Normalize();
				target *= move.Len() * speed;

				if (target != Vec3() && run)
					stamina.current -= dTime * 10.0f;
				else
					stamina.current += dTime * 10.0f * health.current / 100.0f;

				if (stamina.current < 0.0f)
				{
					health.current += stamina.current;
					stamina.current = 0.0f;
				}

				v -= land_v;
				float nv = v.Dot(land_n);
				v -= land_n * nv;

				v = bu_blend(v, target, -0.5f, -40.0f, dTime);

				v += land_n * nv;
				v += land_v;

				if (input.find("jump") != input.end()) {
					
					v -= land_v;
					if (up.Dot(v)<0.0f)
						v -= up * up.Dot(v);
					v += land_v;
					v += up * 4.0f;

					stamina.current -= 10.0f;

					input.erase("jump");
				}

				stamina.current = std::fmaxf(0.0f, std::fminf(stamina.current, stamina.max));
			}

			if (prev != *p)
				if (pc != nullptr)
					pc->update();
		}
	}

	if (weapon != nullptr)
	{
		auto wpc = weapon->entity->getComponent<PositionComponent>();
		if (wpc != nullptr)
		{
			wpc->p = *p + up * 0.45f + Vec3(-0.25f, -0.5f + recoil * 0.25f, 0.75f - recoil * 0.5f) * cam_rot;
			wpc->update();
			auto wgc = weapon->entity->getComponent<GraphicsComponent>();
			if (wgc != nullptr)
			{
				wgc->decs.items.front()->local = cam_rot * Quaternion(recoil, Vec3(-1.0f, 0.0f, 0.0f));
				wgc->decs.update(0);
			}
		}
		recoil -= run;
		recoil *= exp(log(0.05f) * dTime);
		recoil += run;
	}
}

void MobComponent::writeLog(outstream& os, ClientData& client)
{
	os << facing << move_facing << cam_facing << up;
	os << v << land_n << land_v << landed;
	os << health;
}

void MobComponent::readLog(instream& is)
{
	is >> facing >> move_facing >> cam_facing >> up;
	is >> v >> land_n >> land_v >> landed;
	is >> health;
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
	if (mob != nullptr)
	{
		facing = bu_sphere(mob->facing, facing, up, fWeight);
		move_facing = bu_sphere(mob->move_facing, move_facing, up, fWeight);
		cam_facing = bu_sphere(mob->cam_facing, cam_facing, up, fWeight);
		up = bu_sphere(mob->up, up, facing.Cross(up), fWeight);
		v = mob->v;// v*(1.0f - fWeight) + mob->v*fWeight;
		land_n = mob->land_n;
		land_v = mob->land_v;
		landed = mob->landed;
		health = mob->health;
	}
}

void MobComponent::write_to(outstream& os, ClientData& client) const
{
	os << facing << move_facing << cam_facing << up;
}

void MobComponent::write_to(outstream& os) const
{
}
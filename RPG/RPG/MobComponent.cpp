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
#include "GameStateComponent.h"

#include "GlobalPosition.h"

#include "ClientData.h"

#include "Client.h"

#include "Input.h"

#include "BlendUtility.h"

#include "SimpleState.h"
#include "BlendState.h"

#include "ShieldAura.h"

#include "Aura.h"

AutoSerialFactory<MobComponent, Component> MobComponent::_factory("MobComponent");

MobComponent::MobComponent(void) : Component(_factory.id), health(20), stamina(12), mana(4)
{
	up = Vec3(0.0f, 0.0f, 1.0f);
	speed_mod = 1.0f;
	r = 0.5f;
	use_base_collision = false;
	p = nullptr;
	facing.y = M_PI_2;
}

MobComponent::MobComponent(instream& is) : Component(_factory.id)
{
	is >> facing >> up;
	p = nullptr;
}

MobComponent::~MobComponent(void)
{
}

void MobComponent::connect(NewEntity * pEntity, bool authority)
{
	if (authority)
		abilities.setSyncState(&pEntity->ss);
}

void MobComponent::disconnect(void)
{
	abilities.setSyncState(nullptr);
}

#include "Profiler.h"

#include "LineComponent.h"
#include "AudioComponent.h"

void MobComponent::land(float dTime)
{
	Vec3 target = move;
	if (land_n != Vec3())
	{
		Vec3 target_right = target.Cross(up);
		target = land_n.Cross(target_right);
	}
	target.Normalize();

	if (recovering)
		run = false;

	if (!busy)
	{
		if (target != Vec3() && run)
			stamina_regen -= dTime * 5.0f;
	}

	float speed = crouch ? 2.0f : run ? 9.0f : recovering ? 3.0f : 6.0f;

	speed *= speed_mod;

	if (busy)
		speed = 0.0f;

	target *= move.Len() * speed;

	{
		v -= land_v;
		float nv = v.Dot(land_n);
		v -= land_n * nv;

		if (!rolling)
			v = bu_blend(v, target, -0.5f, -40.0f, dTime);

		v += land_n * nv;
		v += land_v;
	}

	if (!recovering && !busy)
	{
		if (input.find("roll") != input.end() && target != Vec3() && stamina.current >= 1)
		{
			acc->set_state(new SimpleState("roll", 2.0f));

			v = target.Normalized() * 10.0f + land_v;

			stamina.current -= 1;

			input.erase("roll");
		}

		if (input.find("jump") != input.end() && stamina.current >= 1)
		{
			v -= land_v;
			if (up.Dot(v) < 0.0f)
				v -= up * up.Dot(v);
			v += land_v;
			v += up * 4.0f;

			stamina.current -= 1;

			input.erase("jump");
		}
	}
}

void MobComponent::tick(float dTime)
{
	if (!p)
		p = entity->getComponent<PositionComponent>();
	if (!acc)
		acc = entity->getComponent<AnimationControlComponent>();

	if (entity->world->authority)
	{
		int64_t freq, start, end;

		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		QueryPerformanceCounter((LARGE_INTEGER*)&start);

		for (auto& aura : auras)
		{
			if ((aura->duration -= dTime) <= 0.0f)
			{
				delete aura;
				aura = nullptr;
			}
		}
		auras.erase(std::remove(auras.begin(), auras.end(), nullptr), auras.end());

		recovering = input.find("recover") != input.end();
		rolling = acc->has_state("roll") || acc->has_state("hit");
		busy = !acc->has_state("run");

		if (acc->has_state("roll"))
		{
			Vec3 vd = land_v - v;
			facing.x = atan2(vd.x, vd.y);
		}

		if (input["switch"] && weapon)
		{
			weapon = weapon->swap(1 - weapon_index);
			weapon_index = 1 - weapon_index;
			input.erase("switch");
		}

		if (input["attack"])
		{
			bool busy = !acc->has_state("run");
			if (!busy && !recovering)
			{
				size_t stamina_cost = 1;
				if (stamina.current >= stamina_cost)
				{
					stamina.current -= stamina_cost;
					acc->set_state(new BlendState("attack", "attack_up", 1.0f - facing.y * 2.0f / float(M_PI), 3.0f));
					input.erase("attack");
				}
			}
		}

		if (input["dash"])
		{
			Vec3 impulse = Vec3(0.0f, 0.0f, 40.0f) * Quaternion(facing.y, Vec3(1.0f, 0.0f, 0.0f)) * Quaternion(-facing.x, Vec3(0.0f, 0.0f, 1.0f));
			v += impulse;
			takeoff_v += impulse;
			input.erase("dash");

			add_aura(new ShieldAura(40, 3.0f));
		}

		if (input["heal"])
		{
			bool busy = !acc->has_state("run");
			if (!busy)
			{
				size_t mana_cost = 1;
				if (mana.current >= mana_cost)
				{
					auto quaff = new SimpleState("quaff", 1.0f);
					quaff->events.insert(std::make_pair(0.85f, [=]()
					{
						if (mana.current >= mana_cost)
						{
							mana.current -= mana_cost;
							do_heal(8, entity->get_id());
						}
					}));
					acc->set_state(quaff);
					input.erase("heal");
				}
			}
		}

		if (!busy)
		{
			if (landed)
				stamina_regen += 4.0f * dTime;
			else
				stamina_regen += 0.5f * dTime;
		}

		int32_t stamina_regen_integer = stamina_regen;
		stamina_regen -= stamina_regen_integer;
		stamina.current += stamina_regen_integer;

		if (stamina.current >= stamina.max)
		{
			stamina_regen = std::fminf(stamina_regen, 0.0f);
		}

		if (stamina.current <= 0)
		{
			if (input.find("recover") == input.end())
			{
				if (temp_team == 0)
				{
					NewEntity * sound_ent = new NewEntity();
					auto audio = new AudioComponent("data/assets/audio/strained_breathing_short.wav");
					audio->pos_id = entity->get_id();
					sound_ent->addComponent(audio);
					entity->world->AddEntity(sound_ent);
				}

				input["recover"] = 3.0f;
			}
		}

		/*if (health.current <= 0)
		{
			entity->world->SetEntity(entity->id, nullptr);
			if (weapon)
				entity->world->SetEntity(weapon->entity->id, nullptr);
			if (on_death)
				on_death();
		}*/

		for (auto i = input.begin(); i != input.end();)
		{
			i->second -= dTime;
			if (i->second <= 0.0f)
				i = input.erase(i);
			else
				++i;
		}

		if (move.Len() > 1.0f)
			move.Normalize();

		if (p != nullptr)
		{
			if (Vec3(p->p).z < 0.0f)
			{
				if (temp_team == 0)
				{
					p->p = respawn;
					v = Vec3();
					for (auto& r : resource)
						r.current = r.max;
				}
				else
				{
					entity->world->SetEntity(entity->id, nullptr);
				}
			}

			if (use_base_collision)
			{
				auto mobs = entity->world->GetNearestComponents<MobComponent>(p->p);
				for each (auto mob in mobs)
				{
					if (mob.second->use_base_collision)
					{
						float r_plus_r = r + mob.second->r;
						if (mob.first < r_plus_r && mob.second != this)
						{
							Vec3 dif = p->p - mob.second->p->p;
							float l = mob.first;
							Vec3 dir = dif;
							if (l != 0.0f)
								dir /= l;

							Vec3 vdif = v - mob.second->v;
							vdif = dif * dif.Dot(vdif);

							v -= vdif * 0.5f;
							mob.second->v += vdif * 0.5f;

							dif *= r_plus_r - l;
							external_dp += dif * 0.5f;
							mob.second->external_dp -= dif * 0.5f;
						}
					}
				}
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

			GlobalPosition prev = p->p;

			//v += move * dTime * 100.0f;

			landed = false;
			//land_n = Vec3(); // smooth these out for some cases
			//land_v = Vec3(); // -''-

			float t = 1.0f;

			bool done_landing = false;

			Vec3 g_dir = Vec3(0.0f, 0.0f, -1.0f);//-Vec3(*p).Normalized();
			Vec3 g = g_dir * 9.8f;

			float immersion = fminf(1.5f, 1.0f - Vec3(p->p).z) / 1.5f;
			if (immersion > 0.0f)
			{
				takeoff_v = Vec3();
				land_n = Vec3();
				land_v = Vec3();
				g -= g * (1.0f / 0.98f) * immersion;
			}

			dp = (g / 2.0f * dTime + v) * dTime + external_dp;

			std::set<void*> ignored;
			std::set<Vec3> previous;

			/*auto wpc = weapon->entity->getComponent<PositionComponent>();
			if (wpc != nullptr)
			{
				wpc->p = *p;
				wpc->update();
			}*/

			while (t > 0.0f)
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

					float disk_radius = 0.5f * r;
					float offset = 0.0f;
					float standing_height = (crouch ? 1.0f : 2.0f) * r;
					float height = standing_height;
					Vec3 dp_side = dp - up * up.Dot(dp);
					ColliderComponent::DiskCast(p->p - up * offset, p->p - up * (offset + height), disk_radius, list);
					if (!list.empty())
					{
						/*list.erase(std::remove_if(list.begin(), list.end(), [=](const std::shared_ptr<Collision>& col)
						{
							return col->n.z <= 0.5f;
						}), list.end());*/
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
							Vec3 v = list.front()->v;
							list.clear();

							if (line != 0)
							{
								line->lines[0].first = pivot - p->p;
								line->lines[0].second = pivot + up - p->p;
								line->color[0].first = Vec3(1.0f, 1.0f, 1.0f);
								line->color[0].second = Vec3(1.0f, 0.0f, 0.0f);

								line->lines[3].first = pivot - p->p;
								line->lines[3].second = center - p->p;
								line->color[3].first = Vec3(1.0f, 0.0f, 0.0f);
								line->color[3].second = Vec3(1.0f, 0.0f, 0.0f);

								line->lines[6].first = pivot - p->p;
								line->lines[6].second = pivot + axis - p->p;
								line->color[6].first = Vec3(1.0f, 0.0f, 0.0f);
								line->color[6].second = Vec3(0.0f, 0.0f, 0.0f);
							}

							if (n.z > 0.5f)
							{
								float height_mod = (tdt - 1.0f) * 0.5f;
								if (pivot == center)
								{
									std::shared_ptr<Collision> col(new Collision());
									col->t = 0.0f;
									col->n = n;
									col->poo = center + up * (standing_height + height_mod);
									col->v = v;
									list.push_back(col);
								}
								else
								{
									std::shared_ptr<Collision> col(new Collision());
									col->t = 0.0f;
									col->n = n;
									col->poo = center + up * (standing_height + height_mod);
									col->v = v;
									list.push_back(col);
								}
							}
						}
					}

					if (!list.empty())
					{
						land_n = list.front()->n;
						land_v = list.front()->v;
					}
				}

				/*for (auto i = list.begin(); i != list.end(); ++i)
				{
					(*i)->poo += up * 0.75f;
					(*i)->n = up;
				}*/

				ColliderComponent::SphereCast(p->p, p->p + dp, r, list);
				
				std::shared_ptr<Collision> col;
				for (auto i = list.begin(); i != list.end(); ++i)
				{
					if (ignored.find((*i)->wall) == ignored.end())
					{
						if ((*i)->t >= 0.0f && (*i)->t <= 1.0f)
						{
							if ((*i)->n.Dot(dp - (*i)->v * dTime) < 0.0f)
							{
								if (col != 0)
								{
									if (col->t > (*i)->t)
										col = *i;
								}
								else
								{
									col = *i;
								}
							}
						}
					}
				}

				if (col != 0)
				{
					t -= col->t;

					v += g * dTime * col->t;

					p->p = col->poo;

					v -= col->v;

					if (col->n.Dot(up) > 0.5f)
						landed = true;

					{
						float fall_one = col->n.Dot(v) / 10.0f;
						float fall_damage_part = (fall_one * fall_one - 1.0f) / 10.0f;
						if (fall_damage_part > 0.0f)
						{
							uint32_t fall_damage = health.max * fall_damage_part;
							if (fall_damage > 0)
							{
								hit = true;
								do_damage(HitData(fall_damage));
							}
						}
					}

					if (col->n.Dot(up) > land_n.Dot(up) || ignored.empty())
					{
						land_n = col->n;
						land_v = col->v;
					}

					float v_dot_n = v.Dot(col->n);
					if (v_dot_n < 0.0f)
						v -= col->n * v_dot_n;
					v += col->v;

					if (!done_landing)
					{
						if (crouch)
							run = false;

						if (landed)
						{
							done_landing = true;

							land(dTime);
						}
					}

					takeoff_v = (v - land_v) * 0.6f + land_v;

					float edp_dot_n = external_dp.Dot(col->n);
					if (edp_dot_n < 0.0f)
						external_dp -= col->n * edp_dot_n;

					dp = (g / 2.0f * dTime * t + v) * dTime * t + external_dp;

					bool should_break = false;
					for (auto i = previous.begin(); i != previous.end(); ++i)
						if (i->Dot(dp) < 0.0f)
							should_break = true;

					if (should_break)
						break;

					ignored.insert(col->wall);
				}
				else
				{
					p->p += dp;
					v += g * dTime*t;
					break;
				}
			}

			if (immersion > 0.0f)
			{
				v += move_space * 6.0f * dTime;
				v = bu_blend(v, Vec3(), log(0.1f) * immersion, 0.0f, dTime);
			}
			else
			{
				if (!done_landing)
				{
					v -= takeoff_v;
					float z = v.z;
					v.z -= z;

					if (!rolling)
						v = bu_blend(v, move * 6.0f * 0.4f, -0.5f, -5.0f, dTime);

					v.z += z;
					v += takeoff_v;
				}
			}

			dp = Vec3();
			external_dp = Vec3();

			if (prev != p->p)
				p->update();
		}

		QueryPerformanceCounter((LARGE_INTEGER*)&end);

		Profiler::set("mob", 1.0 / (static_cast<double>(end - start) / freq));
	}
}

void MobComponent::writeLog(outstream& os, const std::shared_ptr<ClientData>& client)
{
	os << facing << up;
	os << v << land_n << land_v << landed;
	os << health << stamina << mana;
	os << run << crouch;
	os << move;
	os << input;
	os << r;
	abilities.writeLog(os);
}

void MobComponent::readLog(instream& is)
{
	is >> facing >> up;
	is >> v >> land_n >> land_v >> landed;
	is >> health >> stamina >> mana;
	is >> run >> crouch;
	is >> move;
	is >> input;
	is >> r;
	abilities.readLog(is);
}

void MobComponent::writeLog(outstream& os)
{
}

void MobComponent::readLog(instream& is, const std::shared_ptr<ClientData>& client)
{
}

void MobComponent::interpolate(Component * pComponent, float fWeight)
{
	auto mob = reinterpret_cast<MobComponent*>(pComponent);
	if (mob != nullptr)
	{
		facing.x = bu_angle(mob->facing.x, facing.x, fWeight);
		facing.y = bu_blend(facing.y, mob->facing.y, fWeight);
		up = bu_sphere(mob->up, up, Vec3(), fWeight);
		v = bu_blend(v, mob->v, fWeight);// v*(1.0f - fWeight) + mob->v*fWeight;
		land_n = bu_blend(land_n, mob->land_n, fWeight);
		land_v = bu_blend(land_v, mob->land_v, fWeight);
		landed = bu_blend(landed, mob->landed, fWeight);
		health = mob->health;
		stamina = mob->stamina;
		mana = mob->mana;
		run = mob->run;
		crouch = mob->crouch;
		move = bu_blend(move, mob->move, fWeight);
		input = mob->input;
		r = mob->r;
		abilities = mob->abilities;
	}
}

void MobComponent::write_to(outstream& os, const std::shared_ptr<ClientData>& client) const
{
	os << facing << up;
}

void MobComponent::write_to(outstream& os) const
{
}

#include "InventoryComponent.h"

void MobComponent::do_damage(HitData& hit)
{
	if (hit.source.id != 0xffffffff)
		last_hit = hit.source;

	auto source_entity = entity->world->GetEntity(last_hit);
	if (source_entity)
	{
		auto source_mob = source_entity->getComponent<MobComponent>();
		if (source_mob)
		{
		}
	}

	on_hit_taken.call(hit);

	uint32_t damage = hit.getTotal();

	if (damage > 0)
	{
		health.current -= damage;
		auto inv = entity->getComponent<InventoryComponent>();
		if (inv)
			inv->hp_changes.queue.push_back(damage);
		if (health.current <= 0)
		{
			on_fatal_hit_taken.call(hit);

			damage = hit.getTotal();

			if (damage > 0)
			{
				on_death.call(hit);

				if (temp_team == 0)
				{
					p->p = respawn;
					v = Vec3();
					for (auto& r : resource)
						r.current = r.max;
				}
				else
				{
					entity->world->SetEntity(entity->id, nullptr);
				}
			}
		}
	}

	if (entity->world->authority)
	{
		if (temp_team == 0)
		{
			NewEntity * sound_ent = new NewEntity();
			auto audio = new AudioComponent("data/assets/audio/ouch.wav");
			audio->pos_id = entity->get_id();
			sound_ent->addComponent(audio);
			entity->world->AddEntity(sound_ent);
		}

		if (temp_team == 1)
		{
			NewEntity * sound_ent = new NewEntity();
			auto audio = new AudioComponent("data/assets/audio/ZombieOuch.wav");
			audio->pos_id = entity->get_id();
			sound_ent->addComponent(audio);
			entity->world->AddEntity(sound_ent);
		}
	}
}

void MobComponent::do_heal(size_t heal, EntityID source)
{
	health.current += heal;
	if (health.current > health.max)
		health.current = health.max;
}

void MobComponent::add_aura(Aura * aura)
{
	auras.push_back(aura);
	aura->attach(this);
}

void MobComponent::remove_aura(Aura * aura)
{
	auto i = std::find(auras.begin(), auras.end(), aura);
	delete *i;
	auras.erase(i);
}
#include "AttackCap.h"
#include "PlayerUnit.h"
#include "GruntUnit.h"

#include "Server.h"
#include "Client.h"
#include "World.h"
//extern Server * server;
//extern Client * client;

#include "APL.h"

AttackCap::AttackCap(Unit * unit, const std::string& script) : UnitCapability(unit), _script(script)
{
	Resource::load(_script);
}

AttackCap::~AttackCap(void)
{
}

#include "StringResource.h"
#include "FloatVar.h"
#include "StringVar.h"

void AttackCap::tick(float dTime)
{
	if (mem==0)
	{
		auto text = Resource::get<StringResource>(_script);
		if (text!=0)
		{
			mem.reset(new ScriptMemory());
			Script script(std::istringstream(text->string));
			script.run(mem);
		}
	}

	Unit * pu = dynamic_cast<Unit*>(unit);
	PlayerUnit * player = dynamic_cast<PlayerUnit*>(unit);
	GruntUnit * grunt = dynamic_cast<GruntUnit*>(unit);
	
	std::weak_ptr<AnimationPoseLayer> * ua;
	std::shared_ptr<APLList> apl_list;
	if (player!=0) {
		ua = &player->current_action;
		apl_list = player->upper_apl;
	}
	if (grunt!=0) {
		ua = &grunt->current_action;
		apl_list = grunt->apl_list;
	}//TODO make better system for this

	std::string _anim, _combo;
	if (mem!=0)
	{
		auto var = std::dynamic_pointer_cast<StringVar>(mem->getVariable("animation"));
		if (var!=0)
			_anim = var->str;
		var = std::dynamic_pointer_cast<StringVar>(mem->getVariable("combo"));
		if (var!=0)
			_combo = var->str;

		if (combo==0 && _combo.size())
		{
			combo.reset(new AttackCap(unit, _combo));
		}
	}

	if (pu!=0 && ua!=0 && apl_list!=0)
	{
		if (world->authority)
		{
			if (pu->cs.active.find("attack")!=pu->cs.active.end()) {
				if (player!=0)
					player->combat_timer = 0.0f;
				if (ua->expired() && action.expired())
				{
					pu->cs.active.erase("attack");
					if (!pu->dead)
					{
						std::shared_ptr<SkeletalAnimation> anim = Resource::get<SkeletalAnimation>(pu->rAnimation);
						if (anim!=0 && pu->apl!=0)
						{
							pu->cs.activate(_anim);//TODO make better action control naming system
						}
					}
				}
			}
		}
		if (pu->cs.active.find(_anim)!=pu->cs.active.end()) {
			pu->cs.active.erase(_anim);
			std::shared_ptr<SkeletalAnimation> anim = Resource::get<SkeletalAnimation>(pu->rAnimation);
			if (anim!=0 && pu->apl!=0)
			{
				float slice_width = 0.5f;
				float slice_dir = 0.0f;
				float slice_reach = 1.0f;
				float slice_size = 1.0f;
				float slice_start = 0.0f;
				float slice_end = 60.0f;
				float knockback = 0.0f;
				float knockup = 0.0f;
				float speed = 60.0f;
				float blend_in = 0.0f;
				float blend_out = 0.0f;
				float combo_start = 0.0f;
				float combo_end = 0.0f;
				if (mem!=0)
				{
					auto var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("slice_width"));
					if (var!=0)
						slice_width = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("slice_dir"));
					if (var!=0)
						slice_dir = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("slice_reach"));
					if (var!=0)
						slice_reach = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("slice_size"));
					if (var!=0)
						slice_size = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("slice_start"));
					if (var!=0)
						slice_start = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("slice_end"));
					if (var!=0)
						slice_end = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("knockback"));
					if (var!=0)
						knockback = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("knockup"));
					if (var!=0)
						knockup = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("speed"));
					if (var!=0)
						speed = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("blend_in"));
					if (var!=0)
						blend_in = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("blend_out"));
					if (var!=0)
						blend_out = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("combo_start"));
					if (var!=0)
						combo_start = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("combo_end"));
					if (var!=0)
						combo_end = var->f;
				}

				auto src = std::make_shared<APLSource>(&anim->actions[_anim], false);
				auto layer = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 1.0f, std::make_shared<APLSpeed>(speed, src));
				std::shared_ptr<std::set<Entity*>> set(new std::set<Entity*>());
				set->insert(pu);

				std::weak_ptr<APLSource> w_src(src);
				std::weak_ptr<APLBlend> w_layer(layer);

				src->on_tick = [=] (float dTime) -> bool {
					if (pu->dead)
						return false;

					auto src = w_src.lock();
					auto layer = w_layer.lock();
					layer->weight = std::min(1.0f, std::min(blend_in==0.0f ? 1.0f : src->time/blend_in, blend_out==0.0f ? 1.0f : (src->action->length-src->time)/blend_out));
					if (world->authority)
					{
						if (src->time>=slice_start && src->time<=slice_end) {
							std::vector<std::shared_ptr<Collision>> list;
							float f = (M_PI*(src->time-5.0f)/(slice_end-slice_start)-M_PI/2.0f+slice_dir)*slice_width;
							world->UnitCheck(pu->p+Vec3(-sin(pu->facing.x+f), cos(pu->facing.x+f), 0.0f)*slice_reach*pu->scale, slice_size*pu->scale, list);
							for (auto i=list.begin();i!=list.end();++i)
							{
								if (set->find((*i)->entity)==set->end())
								{
									set->insert((*i)->entity);
									Unit * unit = dynamic_cast<Unit*>((*i)->entity);
									if (unit!=0)
									{
										int damage = 1;
										if (mem!=0)
										{
											auto var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("damage"));
											if (var!=0)
												damage = var->f;
										}

										unit->OnHitBy(DamageSource(pu, damage));
										Vec3 dif = unit->p-pu->p;
										dif.Normalize();
										unit->v += dif*knockback;
										unit->v.z += knockup;
									}
								}
							}
						}
					}
					if (src->time>=combo_start && src->time<=combo_end)
					{
						auto lock_action = ua->lock();
						if (lock_action==layer)
							ua->reset();
						if (combo!=0)
							combo->tick(dTime);
					}
					return true;
				};
				action = layer;
				*ua = layer;
				apl_list->layers.push_back(layer);

				/*auto list = std::make_shared<APLList>();
				auto layer = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 1.0f, list);
				auto src = std::make_shared<APLSource>(&anim->actions["bow_draw"], false);
				auto src_up = std::make_shared<APLBlend>(std::make_shared<Pose>(), 0.0f, std::make_shared<APLSource>(&anim->actions["bow_draw_up"], false));
				auto src_down = std::make_shared<APLBlend>(std::make_shared<Pose>(), 0.0f, std::make_shared<APLSource>(&anim->actions["bow_draw_down"], false));

				src->on_tick = [this, src, src_up, src_down, layer] (float dTime) -> bool {
				layer->weight = std::min(1.0f, src->time/10.0f);
				src_up->weight = std::max<float>(0.0f, this->facing.y/M_PI*2.0f);
				src_down->weight = std::max<float>(0.0f, -this->facing.y/M_PI*2.0f);
				return true;
				};

				if (world->authority)
				{

				}
				list->layers.push_back(src);
				list->layers.push_back(src_up);
				list->layers.push_back(src_down);

				upper_apl->layers.push_back(layer);*/
			}
		}
	}
}
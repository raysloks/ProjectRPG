#include "BlockCap.h"
#include "PlayerUnit.h"

#include "Server.h"
#include "Client.h"
#include "World.h"
extern Server * server;
extern Client * client;

#include "APL.h"

BlockCap::BlockCap(Unit * unit, const std::string& script) : UnitCapability(unit), _script(script)
{
	Resource::load(_script);
	cd = 0.0f;
}

BlockCap::~BlockCap(void)
{
}

#include "StringResource.h"
#include "FloatVar.h"
#include "StringVar.h"

void BlockCap::tick(float dTime)
{
	if (cd>0.0f)
		cd -= dTime;

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
	
	std::weak_ptr<AnimationPoseLayer> * ua;
	std::shared_ptr<APLList> apl_list;
	if (player!=0) {
		ua = &player->current_action;
		apl_list = player->upper_apl;
	}

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
			//combo.reset(new AttackCap(unit, _combo));
		}
	}

	if (pu!=0 && ua!=0 && apl_list!=0)
	{
		if (world->authority)
		{
			if (pu->cs.active.find("block")!=pu->cs.active.end()) {
				if (player!=0)
					player->combat_timer = 0.0f;
				if (ua->expired() && action.expired() && cd<=0.0f)
				{
					pu->cs.active.erase("block");
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
				float speed = 60.0f;
				float blend_in = 0.0f;
				float blend_out = 0.0f;
				float block_start = 0.0f;
				float block_end = 0.0f;
				float block_cover = 0.0f;
				float cooldown = 0.0f;
				if (mem!=0)
				{
					auto var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("speed"));
					if (var!=0)
						speed = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("blend_in"));
					if (var!=0)
						blend_in = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("blend_out"));
					if (var!=0)
						blend_out = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("block_start"));
					if (var!=0)
						block_start = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("block_end"));
					if (var!=0)
						block_end = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("block_cover"));
					if (var!=0)
						block_cover = var->f;
					var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("cooldown"));
					if (var!=0)
						cooldown = var->f;
				}

				cd = cooldown;

				auto src = std::make_shared<APLSource>(&anim->actions[_anim], false);
				auto layer = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 1.0f, std::make_shared<APLSpeed>(speed, src));
				std::shared_ptr<std::set<Entity*>> set(new std::set<Entity*>());
				set->insert(pu);

				std::weak_ptr<APLSource> w_src(src);
				std::weak_ptr<APLBlend> w_layer(layer);

				std::shared_ptr<std::function<void(DamageSource&)>> onhit(new std::function<void(DamageSource&)>([this, block_start, block_end, w_src, w_layer, ua] (DamageSource& dsrc) {
					auto src = w_src.lock();
					auto layer = w_layer.lock();

					if (src->time>block_start && src->time<block_end)
					{
						dsrc.damage = 0;
						cd = 0;

						auto ua_lock = ua->lock();
						if (ua_lock==layer)
							ua->reset();
						action.reset();
					}
				}));

				if (world->authority)
					pu->onhit.insert(pu->onhit.begin(), onhit);

				src->on_tick = [w_src, w_layer, pu, blend_in, blend_out, onhit] (float dTime) -> bool {
					if (pu->dead)
						return false;

					auto src = w_src.lock();
					auto layer = w_layer.lock();
					layer->weight = std::min(1.0f, std::min(blend_in==0.0f ? 1.0f : src->time/blend_in, blend_out==0.0f ? 1.0f : (src->action->length-src->time)/blend_out));
					return true;
				};

				action = layer;
				*ua = layer;
				apl_list->layers.push_back(layer);
			}
		}
	}
}
#include "JumpCap.h"
#include "PlayerUnit.h"
#include "ParticleSystem.h"
#include "World.h"

JumpCap::JumpCap(Unit * u, float p) : UnitCapability(u), power(p)
{
	peak = 0.0f;
}

JumpCap::~JumpCap(void)
{
}

#include "APL.h"

void JumpCap::tick(float dTime)
{
	PlayerUnit * pu = dynamic_cast<PlayerUnit*>(unit);
	if (pu!=0)
	{
		if (pu->cs.active.find("jump")!=pu->cs.active.end()) {
			pu->cs.active.erase("jump");
			if (!pu->dead) {
				if (pu->time_since_landed<=0.2f && pu->time_since_jump>0.2f)
				{
					//world->AddEntity(new ParticleSystem(pu->p));

					pu->time_since_jump = 0.0f;
					/*pu->v += pu->land_n * power * std::max(0.0f, pu->target.Dot(pu->land_n));
					pu->v.z += power;*/
					pu->v -= pu->land_v;
					if (pu->v.z < 0.0f)
						pu->v.z = 0.0f;
					pu->v.z += power;//*std::min(1.0f, 1.5f-std::max(0.5f, -pu->cs.input["dir_y"]*1.5f*2.0f/(float)M_PI));
					pu->v += pu->land_v;

					std::shared_ptr<SkeletalAnimation> anim = Resource::get<SkeletalAnimation>(pu->rAnimation);

					float blend_in = 0.0f;
					float blend_out = 10.0f;

					if (anim!=0 && pu->apl!=0)
					{
						auto src = std::make_shared<APLSource>(&anim->actions["jump"], false);
						auto blend = std::make_shared<APLBlend>(std::make_shared<Pose>(anim->armature), 1.0f, std::make_shared<APLSpeed>(60.0f, src));
						auto list = std::make_shared<APLList>();
						list->layers.push_back(std::make_shared<APLStatic>(std::make_shared<Pose>(anim->armature)));
						list->layers.push_back(blend);
						auto layer = std::make_shared<APLAdd>(std::make_shared<Pose>(anim->armature), list);

						std::weak_ptr<APLSource> w_src(src);
						std::weak_ptr<APLBlend> w_blend(blend);

						src->on_tick = [=] (float dTime) -> bool {
							auto src = w_src.lock();
							auto blend = w_blend.lock();
							blend->weight = std::min(1.0f, std::min(blend_in==0.0f ? 1.0f : src->time/blend_in, blend_out==0.0f ? 1.0f : (src->action->length-src->time)/blend_out));
							return true;
						};

						pu->apl_list->layers.push_back(layer);
					}
				}
			}
		}
	}
}
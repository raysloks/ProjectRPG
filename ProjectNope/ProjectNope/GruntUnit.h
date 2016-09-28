#ifndef GRUNT_UNIT_H
#define GRUNT_UNIT_H

#include "Unit.h"

class APLList;
class APLBlend;
class APLSpeed;
class GruntAI;

class GruntUnit :
	public Unit
{
public:
	GruntUnit(const GlobalPosition& pos, const Vec3& vel);
	GruntUnit(instream& is, bool full);
	~GruntUnit(void);

	static const AutoSerialFactory<GruntUnit> _factory_GruntUnit;

	virtual void tick(float dTime);

	std::shared_ptr<APLList> apl_list;
	std::shared_ptr<APLBlend> fall_blend_apl, walk_blend_apl;
	std::shared_ptr<APLSpeed> walk_spd_apl;

	std::weak_ptr<AnimationPoseLayer> current_action;

	virtual void _initAPL(const std::shared_ptr<SkeletalAnimation>& anim);
	virtual void _tickAPL(const std::shared_ptr<SkeletalAnimation>& anim, float dTime);

	std::shared_ptr<GruntAI> ai;
};

#endif
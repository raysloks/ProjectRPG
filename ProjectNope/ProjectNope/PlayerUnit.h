#ifndef PLAYER_UNIT_H
#define PLAYER_UNIT_H

#include "Unit.h"

#include "APLSet.h"

class Mesh;
class AnimationPoseLayer;
class APLList;
class APLBlend;
class APLSpeed;
class Pose;
class SkeletalAnimation;
class PlayerSaveState;
class Item;

class PlayerUnit :
	public Unit
{
public:
	PlayerUnit(const GlobalPosition& pos, const Vec3& vel);
	PlayerUnit(instream& is, bool full);
	~PlayerUnit(void);

	virtual void writeLog(outstream& os);
	virtual void readLog(instream& is);

	static const AutoSerialFactory<PlayerUnit> _factory_PlayerUnit;

	virtual void tick(float dTime);
	virtual void render(const GlobalPosition& origin);

	virtual void interpolate(Entity * pEntity, float fWeight);

	virtual void write_to(outstream& os, bool full) const;

	std::shared_ptr<APLList> apl_list, lower_apl, upper_apl;
	std::shared_ptr<APLBlend> arms_walk_blend_apl, fall_blend_apl, walk_blend_apl, right_blend_apl, left_blend_apl, turn_right_blend_apl, turn_left_blend_apl, tilt_blend_apl;
	std::shared_ptr<APLSpeed> fall_spd_apl, arms_walk_spd_apl, walk_spd_apl, turn_spd_apl;

	std::shared_ptr<PlayerSaveState> save;

	Vec3 last_v;
	float time_since_jump;

	float combat_timer;
//protected:
	std::weak_ptr<AnimationPoseLayer> current_action;

	virtual void _initAPL(const std::shared_ptr<SkeletalAnimation>& anim);
	virtual void _tickAPL(const std::shared_ptr<SkeletalAnimation>& anim, float dTime);

	void equip(const std::shared_ptr<Item>& item);
	void unequip(const std::shared_ptr<Item>& item);
};

#endif
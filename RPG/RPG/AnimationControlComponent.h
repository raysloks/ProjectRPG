#ifndef ANIMATION_CONTROL_COMPONENT_H
#define ANIMATION_CONTROL_COMPONENT_H

#include "Component.h"

#include <memory>

#include "Vec3.h"

#include "Synchronizer.h"

#include "ControlState.h"

class SkeletalAnimation;
class Pose;

class AnimationPoseLayer;
class APLList;
class APLAdd;
class APLBlend;
class APLSpeed;
class APLSource;

class MobComponent;

class AnimationControlComponent :
	public Component
{
public:
	AnimationControlComponent(void);
	AnimationControlComponent(instream& is, bool full);
	~AnimationControlComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void tick(float dTime);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, ClientData& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	static const AutoSerialFactory<AnimationControlComponent> _factory;

	void initAPL(void);
	void tickAPL(float dTime);

	bool start_action(const std::string& fname);

	MobComponent * mob;

	Synchronizer synchro;

	ControlState cs;

	std::shared_ptr<AnimationPoseLayer> apl;
	std::shared_ptr<APLList> apl_list, lower_apl, upper_apl, action_apl;
	std::shared_ptr<APLBlend> arms_walk_blend_apl, idle_blend_apl, fall_blend_apl, walk_blend_apl, run_blend_apl, right_blend_apl, left_blend_apl, back_blend_apl, back_right_blend_apl, back_left_blend_apl, tilt_blend_apl;
	std::shared_ptr<APLSpeed> fall_spd_apl, arms_walk_spd_apl, walk_spd_apl, turn_spd_apl;
	std::shared_ptr<APLSource> turn_right_src_apl, turn_left_src_apl;

	float back_limit;

	Vec3 vp, vb;
	float time_since_landed;

	std::shared_ptr<SkeletalAnimation> anim;
	std::shared_ptr<Pose> pose;
};

#endif
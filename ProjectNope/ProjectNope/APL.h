#include "AnimationPoseLayer.h"
#include <list>

#ifndef APL_H
#define APL_H

class APLBlend :
	public AnimationPoseLayer
{
public:
	APLBlend(std::shared_ptr<Pose> pose, float weight, std::shared_ptr<AnimationPoseLayer> source);
	~APLBlend(void);

	bool tick(float dTime, Pose * pose);
	
	float weight;
	std::shared_ptr<Pose> pose;
	std::shared_ptr<AnimationPoseLayer> source;
};

class APLAdd :
	public AnimationPoseLayer
{
public:
	APLAdd(std::shared_ptr<Pose> pose, std::shared_ptr<AnimationPoseLayer> source);
	~APLAdd(void);

	bool tick(float dTime, Pose * pose);
	
	std::shared_ptr<Pose> pose;
	std::shared_ptr<AnimationPoseLayer> source;
};

class APLList :
	public AnimationPoseLayer
{
public:
	APLList(void);
	~APLList(void);

	bool tick(float dTime, Pose * pose);
	
	std::list<std::shared_ptr<AnimationPoseLayer>> layers;
};

class APLSpeed :
	public AnimationPoseLayer
{
public:
	APLSpeed(float speed, std::shared_ptr<AnimationPoseLayer> source);
	~APLSpeed(void);

	bool tick(float dTime, Pose * pose);
	
	float speed;
	std::shared_ptr<AnimationPoseLayer> source;
};

class APLSource :
	public AnimationPoseLayer
{
public:
	APLSource(Action * action, bool loop);
	APLSource(Action * action, const std::function<bool(void)>& loop_func);
	~APLSource(void);

	bool tick(float dTime, Pose * pose);
	
	float time;
	bool loop;
	Action * action;
	std::function<bool(void)> on_end;
	std::function<bool(float)> on_tick;
};

class APLStatic :
	public AnimationPoseLayer
{
public:
	APLStatic(std::shared_ptr<Pose> pose);
	~APLStatic(void);

	bool tick(float dTime, Pose * pose);
	
	std::shared_ptr<Pose> pose;
};

#endif
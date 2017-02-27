#include "APL.h"

APLBlend::APLBlend(std::shared_ptr<Pose> pse, float w, std::shared_ptr<AnimationPoseLayer> src)
{
	source = src;
	weight = w;
	pose = pse;
}

APLBlend::~APLBlend(void)
{
}

bool APLBlend::tick(float dTime, Pose * out_pose) //TODO optimize for 1.0f and 0.0f and option to base blend factor on dTime
{
	if (!source->tick(dTime, pose.get()))
		return false;
	out_pose->interpolate(*pose, weight);
	return true;
}

APLAdd::APLAdd(std::shared_ptr<Pose> pse, std::shared_ptr<AnimationPoseLayer> src)
{
	source = src;
	pose = pse;
}

APLAdd::~APLAdd(void)
{
}

bool APLAdd::tick(float dTime, Pose * out_pose)
{
	if (!source->tick(dTime, pose.get()))
		return false;
	out_pose->add(*pose);
	return true;
}

APLList::APLList(void){}
APLList::~APLList(void){}

bool APLList::tick(float dTime, Pose * out_pose)
{
	std::vector<std::list<std::shared_ptr<AnimationPoseLayer>>::iterator> rem;
	for (auto it=layers.begin();it!=layers.end();++it)
	{
		if (!(*it)->tick(dTime, out_pose))
			rem.push_back(it);
	}
	for (auto it=rem.begin();it!=rem.end();++it)
		layers.erase(*it);
	return layers.size()>0;
}

APLSource::APLSource(Action * act, bool b) : action(act), loop(b), time(0.0f)
{
}
APLSource::APLSource(Action * act, const std::function<bool(void)>& loop_func) : action(act), on_end(loop_func), time(0.0f)
{
}
APLSource::~APLSource(void)
{
}

bool APLSource::tick(float dTime, Pose * out_pose)
{
	time += dTime;
	if (action == 0)
		return false;
	if (on_tick != 0)
		if (!on_tick(dTime))
			return false;
	if (time>action->length || time<0.0f)
	{
		bool dont_skip = true;
		if (on_end != 0)
			dont_skip = on_end();
		if (dont_skip)
		{
			if (loop)
			{
				if (time<0.0f)
					time += action->length;
				else
					time -= action->length;
			}
			else
			{
				return false;
			}
		}
	}
	*out_pose = *action->anim->getPose(time, *action);
	return true;
}

APLSpeed::APLSpeed(float spd, std::shared_ptr<AnimationPoseLayer> src)
{
	source = src;
	speed = spd;
}
APLSpeed::~APLSpeed(void)
{
}

bool APLSpeed::tick(float dTime, Pose * out_pose)
{
	return source->tick(dTime*speed, out_pose);
}

APLStatic::APLStatic(std::shared_ptr<Pose> pse)
{
	pose = pse;
}
APLStatic::~APLStatic(void)
{
}

bool APLStatic::tick(float dTime, Pose * out_pose)
{
	*out_pose = *pose;
	return true;
}
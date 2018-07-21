#include "RunCycleState.h"

#include "AnimationControlComponent.h"
#include "PoseComponent.h"
#include "MobComponent.h"

#include "BlendUtility.h"

const AutoSerialFactory<RunCycleState> RunCycleState::_factory("RunCycleState");

RunCycleState::RunCycleState(const std::string& n, float s, const std::string& in, float is) : CycleState(n, s), idle(in, is), Serializable(_factory.id)
{
	t = 0.0f;
}

RunCycleState::RunCycleState(instream& is, bool full) : CycleState(is, full), idle(is, full), Serializable(_factory.id)
{
}

RunCycleState::~RunCycleState()
{
}

void RunCycleState::enter(AnimationState * prev)
{
	idle.acc = acc;
	idle.pose = pose;
	idle.mob = mob;
}

void RunCycleState::tick(float dTime)
{
	if (mob->move == Vec3() && mob->landed)
	{
		idle.tick(dTime);
		t *= 2.0f;
		t -= floorf(t);
		t *= 0.5f;
		return;
	}

	if (!authority)
		prev_t = t;

	t += dTime * mob->v.Len() * speed / acc->scale;

	if (!mob->landed)
	{
		if (prev_t <= 1.0f && t > 1.0f)
			t = 1.0f;
		if (prev_t <= 0.5f && t > 0.5f)
			t = 0.5f;
	}

	for (auto e : events)
	{
		if (prev_t < e.first && t >= e.first)
			e.second();
	}

	if (mob->landed)
	{
		t -= floorf(t);
	}
	else
	{
		if (t > 1.0f)
			t -= 1.0f;
	}

	prev_t = t;

	auto anim = Resource::get<SkeletalAnimation>(pose->anim);
	if (anim)
	{
		float start = anim->getStart(name);
		float length = anim->getLength(name);

		pose->frame = start + length * t;
	}
}

void RunCycleState::leave(AnimationState * next)
{
}

void RunCycleState::interpolate(AnimationState * other, float fWeight)
{
	RunCycleState * run_cycle = reinterpret_cast<RunCycleState*>(other);
	t = bu_wrap(run_cycle->t, t, fWeight);
	idle.interpolate(&run_cycle->idle, fWeight);
}

void RunCycleState::write_to(outstream& os, bool full) const
{
	CycleState::write_to(os, full);
	idle.write_to(os, full);
}
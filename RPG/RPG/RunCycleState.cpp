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
	Vec3 rel_v = mob->v - mob->land_v;

	rel_v.z = 0.0f;

	if (rel_v == Vec2() && mob->landed == 1.0f)
	{
		idle.tick(dTime);
		t = 0.5f;
		if (prev_t > 0.25f && prev_t <= 0.75f)
			t = 0.0f;
		return;
	}

	prev_t = t;

	t += dTime * (rel_v.Len() + 1.0f) * speed / acc->scale;

	if (!mob->landed)
	{
		if (prev_t <= 0.5f && t > 0.5f)
			t = 0.5f;
		if (prev_t <= 1.0f && t > 1.0f)
			t = 1.0f;
		if (prev_t <= 0.0f && t > 0.0f)
			t = 0.0f;
	}

	for (auto e : events)
	{
		if (prev_t < e.first && t >= e.first)
			e.second();
	}

	t -= floorf(t);

	prev_t = t;

	auto anim = Resource::get<SkeletalAnimation>(pose->anim);
	if (anim)
	{
		float start = anim->getStart(name);
		float length = anim->getLength(name);

		pose->frame = start + length * t;

		pose->pose = anim->getPose(length * t, name);
		Vec3 forward = Vec3(0.0f, 1.0f, 0.0f) * Quaternion(M_PI - mob->facing.x, Vec3(0.0f, 0.0f, 1.0f));
		float forward_dot = forward.Dot(rel_v.Normalized());

		Vec3 right = Vec3(1.0f, 0.0f, 0.0f) * Quaternion(M_PI - mob->facing.x, Vec3(0.0f, 0.0f, 1.0f));
		float right_dot = right.Dot(rel_v.Normalized());

		right_dot *= fabsf(right_dot);

		auto back_pose = anim->getPose(length * t, "run_back");

		back_pose->interpolate(*anim->getPose(length * t, "run_right"), fmaxf(0.0f, fminf(1.0f, -right_dot)));
		back_pose->interpolate(*anim->getPose(length * t, "run_left"), fmaxf(0.0f, fminf(1.0f, right_dot)));

		pose->pose->interpolate(*anim->getPose(length * t, "run_right"), fmaxf(0.0f, right_dot));
		pose->pose->interpolate(*anim->getPose(length * t, "run_left"), fmaxf(0.0f, -right_dot));

		pose->pose->interpolate(*back_pose, fmaxf(0.0f, fminf(1.0f, -forward_dot * 2.0f)));

		pose->pose->interpolate(*anim->getPose(anim->getLength(idle.name), idle.name), fmaxf(0.0f, 1.0f - fmaxf(1.0f - mob->landed, rel_v.Len())));
	}
}

void RunCycleState::leave(AnimationState * next)
{
}

void RunCycleState::interpolate(AnimationState * other, float fWeight)
{
	RunCycleState * run_cycle = reinterpret_cast<RunCycleState*>(other);
	t = bu_wrap(run_cycle->t, t, fWeight);
	t -= floorf(t);
	idle.interpolate(&run_cycle->idle, fWeight);
}

void RunCycleState::write_to(outstream& os, bool full) const
{
	CycleState::write_to(os, full);
	idle.write_to(os, full);
}
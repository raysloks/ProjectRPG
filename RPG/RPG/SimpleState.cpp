#include "SimpleState.h"

#include "AnimationControlComponent.h"
#include "PoseComponent.h"
#include "MobComponent.h"

#include "BlendUtility.h"

AutoSerialFactory<SimpleState, AnimationState> SimpleState::_factory("SimpleState");

SimpleState::SimpleState(const std::string& n, float s) : name(n), speed(s)
{
	t = 0.0f;
	prev_t = 0.0f;
	blend_t = 0.0f;
}

SimpleState::SimpleState(instream& is, bool full)
{
	is >> t >> speed >> name;
	prev_t = t;
	blend_t = 0.0f;
}

SimpleState::~SimpleState()
{
}

void SimpleState::enter(AnimationState * prev)
{
	prev_pose = pose->pose;
}

void SimpleState::tick(float dTime)
{
	if (!authority)
		prev_t = t;

	t += dTime * speed;

	if (t > 1.0f)
	{
		acc->set_state(nullptr);
		acc->overtime = (t - 1.0f) / speed;
	}

	for (auto e : events)
	{
		if (prev_t < e.first && t >= e.first)
			e.second();
	}

	prev_t = t;

	auto anim = Resource::get<SkeletalAnimation>(pose->anim);
	if (anim)
	{
		float start = anim->getStart(name);
		float length = anim->getLength(name);

		pose->frame = start + length * t;

		anim->getPose(length * t, name, pose->pose);

		blend_t += dTime * 10.0f;
		blend_t = fminf(1.0f, blend_t);

		if (blend_t < 1.0f)
			pose->pose.interpolate(prev_pose, 1.0f - blend_t);
	}
}

void SimpleState::leave(AnimationState * next)
{
}

void SimpleState::interpolate(AnimationState * other, float fWeight)
{
	SimpleState * simple = reinterpret_cast<SimpleState*>(other);
	t = bu_blend(t, simple->t, fWeight);
}

void SimpleState::write_to(outstream& os) const
{
	os << t << speed << name;
}

uint32_t SimpleState::getSerial() const
{
	return _factory.id;
}
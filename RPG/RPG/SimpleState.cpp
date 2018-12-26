#include "SimpleState.h"

#include "AnimationControlComponent.h"
#include "PoseComponent.h"
#include "MobComponent.h"

#include "BlendUtility.h"

ASF_C(SimpleState, AnimationState)

SimpleState::SimpleState(const std::string& n, float s) : name(n), speed(s), AnimationState(_factory.id)
{
	_serial_id = _factory.id;
	t = 0.0f;
	prev_t = 0.0f;
}

SimpleState::SimpleState(instream& is, bool full) : AnimationState(_factory.id)
{
	_serial_id = _factory.id;
	is >> t >> speed >> name;
	prev_t = t;
}

SimpleState::~SimpleState()
{
}

void SimpleState::enter(AnimationState * prev)
{
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

		pose->pose = anim->getPose(length * t, name);
	}
}

void SimpleState::leave(AnimationState * next)
{
}

void SimpleState::interpolate(AnimationState * other, float fWeight)
{
	SimpleState * simple = reinterpret_cast<SimpleState*>(other);
	t = bu_blend(simple->t, t, fWeight);
}

void SimpleState::write_to(outstream& os) const
{
	os << t << speed << name;
}
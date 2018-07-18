#include "SimpleState.h"

#include "AnimationControlComponent.h"
#include "PoseComponent.h"
#include "MobComponent.h"

#include "BlendUtility.h"

const AutoSerialFactory<SimpleState> SimpleState::_factory("SimpleState");

SimpleState::SimpleState(const std::string& n, float s) : name(n), speed(s), Serializable(_factory.id)
{
	t = 0.0f;
}

SimpleState::SimpleState(instream& is, bool full) : Serializable(_factory.id)
{
	is >> t >> speed >> name;
}

SimpleState::~SimpleState()
{
}

void SimpleState::enter(AnimationState * prev)
{
}

void SimpleState::tick(float dTime)
{
	t += dTime * speed;

	if (t > 1.0f)
	{
		acc->set_state(nullptr);
		acc->overtime = (t - 1.0f) / speed;
	}

	auto anim = Resource::get<SkeletalAnimation>(pose->anim);
	if (anim)
	{
		float start = anim->getStart(name);
		float length = anim->getLength(name);

		pose->frame = start + length * t;
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

void SimpleState::write_to(outstream& os, bool full) const
{
	os << t << speed << name;
}
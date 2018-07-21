#include "CycleState.h"

#include "AnimationControlComponent.h"
#include "PoseComponent.h"
#include "MobComponent.h"

#include "BlendUtility.h"

const AutoSerialFactory<CycleState> CycleState::_factory("CycleState");

CycleState::CycleState(const std::string& n, float s) : SimpleState(n, s), Serializable(_factory.id)
{
	t = 0.0f;
}

CycleState::CycleState(instream& is, bool full) : SimpleState(is, full), Serializable(_factory.id)
{
}

CycleState::~CycleState()
{
}

void CycleState::enter(AnimationState * prev)
{
}

void CycleState::tick(float dTime)
{
	if (!authority)
		prev_t = t;

	t += dTime * speed;

	t -= floorf(t);

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
	}
}

void CycleState::leave(AnimationState * next)
{
}

void CycleState::interpolate(AnimationState * other, float fWeight)
{
	CycleState * cycle = reinterpret_cast<CycleState*>(other);
	t = bu_wrap(cycle->t, t, fWeight);
}

void CycleState::write_to(outstream& os, bool full) const
{
	SimpleState::write_to(os, full);
}
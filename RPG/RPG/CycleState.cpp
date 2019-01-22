#include "CycleState.h"

#include "AnimationControlComponent.h"
#include "PoseComponent.h"
#include "MobComponent.h"

#include "BlendUtility.h"

AutoSerialFactory<CycleState, AnimationState> CycleState::_factory("CycleState");

CycleState::CycleState(const std::string& n, float s) : SimpleState(n, s)
{
	t = 0.0f;
}

CycleState::CycleState(instream& is, bool full) : SimpleState(is, full)
{
}

CycleState::~CycleState()
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

		anim->getPose(length * t, name, pose->pose);

		blend_t += dTime * 10.0f;
		blend_t = fminf(1.0f, blend_t);

		if (blend_t < 1.0f)
			pose->pose.interpolate(prev_pose, 1.0f - blend_t);
	}
}

void CycleState::interpolate(AnimationState * other, float fWeight)
{
	CycleState * cycle = reinterpret_cast<CycleState*>(other);
	t = bu_wrap(cycle->t, t, fWeight);
}

void CycleState::write_to(outstream& os) const
{
	SimpleState::write_to(os);
}

uint32_t CycleState::getSerial() const
{
	return _factory.id;
}
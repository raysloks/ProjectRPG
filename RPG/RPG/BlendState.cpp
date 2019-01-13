#include "BlendState.h"

#include "AnimationControlComponent.h"
#include "PoseComponent.h"
#include "MobComponent.h"

#include "BlendUtility.h"

AutoSerialFactory<BlendState, AnimationState> BlendState::_factory("BlendState");

BlendState::BlendState(const std::string& n, const std::string& sn, float w, float s) : SimpleState(n, s)
{
	second_name = sn;
	weight = w;
	t = 0.0f;
}

BlendState::BlendState(instream& is, bool full) : SimpleState(is, full)
{
	is >> second_name >> weight;
}

BlendState::~BlendState()
{
}

void BlendState::enter(AnimationState * prev)
{
}

void BlendState::tick(float dTime)
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
		float second_length = anim->getLength(second_name);

		pose->frame = start + length * t;

		pose->pose = anim->getPose(length * t, name);
		pose->pose->interpolate(*anim->getPose(second_length * t, second_name), weight);
	}
}

void BlendState::leave(AnimationState * next)
{
}

void BlendState::write_to(outstream& os) const
{
	SimpleState::write_to(os);
	os << second_name << weight;
}

uint32_t BlendState::getSerial() const
{
	return _factory.id;
}
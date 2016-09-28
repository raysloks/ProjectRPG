#ifndef ATTACK_CAP_H
#define ATTACK_CAP_H

#include "UnitCapability.h"
#include <memory>
#include <string>

class AnimationPoseLayer;
class ScriptMemory;

class AttackCap
	: public UnitCapability
{
public:
	AttackCap(Unit * unit, const std::string& script);
	~AttackCap(void);

	void tick(float dTime);

	std::string _script;
	std::shared_ptr<ScriptMemory> mem;
	std::shared_ptr<UnitCapability> combo;
	std::weak_ptr<AnimationPoseLayer> action;
};

#endif
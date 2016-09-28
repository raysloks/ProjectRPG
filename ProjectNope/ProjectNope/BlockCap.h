#ifndef BLOCK_CAP_H
#define BLOCK_CAP_H

#include "UnitCapability.h"
#include <memory>
#include <string>

class AnimationPoseLayer;
class ScriptMemory;

class BlockCap
	: public UnitCapability
{
public:
	BlockCap(Unit * unit, const std::string& script);
	~BlockCap(void);

	void tick(float dTime);

	std::string _script;
	std::shared_ptr<ScriptMemory> mem;
	std::shared_ptr<UnitCapability> combo;
	std::weak_ptr<AnimationPoseLayer> action;

	float cd;
};

#endif
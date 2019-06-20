#pragma once

#include <vector>
#include <memory>
#include <string>

class AbilityContext;
class Effect;

class Ability
{
public:
	Ability();
	~Ability();

	static void init();
	static const Ability * get(uint32_t index);
	static void release();

	void activate(const AbilityContext& ac) const;

	float cast_time;
	//Interruptability interruptability;
	std::string icon;

	std::vector<std::shared_ptr<const Effect>> effects;
};
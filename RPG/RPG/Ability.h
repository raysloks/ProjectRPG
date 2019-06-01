#pragma once

class Ability
{
public:
	Ability();
	~Ability();
  
  void activate(const AbilityContext& ac);

  float cast_time;
  Interruptability interruptability;
  
  std::vector<std::shared_ptr<Effect>> effects;
};
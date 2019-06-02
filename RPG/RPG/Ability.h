#pragma once

class Ability
{
public:
	Ability();
	~Ability();
  
  void activate(const AbilityContext& ac) const;

  float cast_time;
  Interruptability interruptability;
  
  std::vector<std::shared_ptr<const Effect>> effects;
};
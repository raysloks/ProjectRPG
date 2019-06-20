#ifndef SKILL_NODE_H
#define SKILL_NODE_H

class Aura;
class Ability;

class SkillNode
{
public:
	SkillNode();
	~SkillNode();

	Aura * getAura(int level);
	Ability * getAbility(int level);
  
};

#endif
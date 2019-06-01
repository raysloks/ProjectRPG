#ifndef SKILL_NODE_H
#define SKILL_NODE_H

class SkillNode
{
public:
	SkillNode();
	~SkillNode();
  
  Aura * getAura(int level);
  Ability * getAbility(int level);
  
};

#endif
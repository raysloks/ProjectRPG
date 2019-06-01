class Effect
{
public:
	abstract ~Effect() = 0;
	
	abstract void apply(const AbilityContext& ac) = 0;
};
class AuraEffect :
	public Effect
{
public:
	~AuraEffect();
	
	void apply(const AbilityContext& ac);
	
	// maybe just a single aura like wow? maybe that is an optimization they've done so all types of effects are the same size
	std::vector<std::shared_ptr<AuraFactory>> auras;
};
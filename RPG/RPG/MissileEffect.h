class MissileEffect :
	public Effect
{
public:
	~MissileEffect();
	
	void apply(const AbilityContext& ac) const;
	 
	uint32_t missile;
};
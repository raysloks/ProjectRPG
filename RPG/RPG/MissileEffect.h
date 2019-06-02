class MissileEffect :
	public Effect
{
public:
	~MissileEffect();
	
	void apply(const AbilityContext& ac) const;
	
	std::shared_ptr<MissileFactory> missile;
};
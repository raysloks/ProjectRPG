class SphereTargetEffect :
	public Effect
{
public:
	~SphereTargetEffect();
	
	void apply(const AbilityContext& ac) const;
	
	float r;
};
#ifndef UNIT_H
#define UNIT_H

#include "Targetable.h"
#include "HealthBar.h"
#include "ControlState.h"
#include "DamageSource.h"
#include "DecoratorContainer.h"
#include "Inventory.h"
#include <memory>

class Collision;
class UnitCapability;
class Mesh;
class Pose;
class AnimationPoseLayer;
class SkeletalAnimation;

float interpolate_direction(float origin, float target, float percentage);

class Unit :
	public Targetable
{
public:
	Unit(const GlobalPosition& pos, const Vec3& vel);
	Unit(instream& is, bool full);
	~Unit(void);

	virtual void writeLog(outstream& os);
	virtual void readLog(instream& is);

	static const AutoSerialFactory<Unit> _factory_Unit;

	virtual void tick(float dTime);
	virtual void render(const GlobalPosition& origin);

	virtual void interpolate(Entity * pEntity, float fWeight);

	virtual void write_to(outstream& os, bool full) const;

	Vec3 land_n, land_v;
	Vec3 target, mass_shift, mass_shift_delta;
	Vec2 facing;
	float time_since_landed;

	int inc;

	float move_speed, mass_shift_speed;
	float turn_rate, turn_speed;
	float scale, m;

	HealthBar health;
	bool dead;

	EntityID look_target;

	float regen, regen_counter;

	virtual void OnCollision(const std::shared_ptr<Collision>& col, float dTime);
	virtual void OnHitBy(DamageSource& src);

	std::vector<std::weak_ptr<std::function<void(DamageSource& src)>>> onhit;

	std::vector<std::shared_ptr<UnitCapability>> uc;

	ControlState cs;

	std::string rAnimation;
	
	std::shared_ptr<Pose> pose;
	std::shared_ptr<AnimationPoseLayer> apl;

	DecoratorContainer decs;
	Inventory inventory;

	virtual void _initAPL(const std::shared_ptr<SkeletalAnimation>& anim);
	virtual void _tickAPL(const std::shared_ptr<SkeletalAnimation>& anim, float dTime);
private:
	void _landed(float dTime);//mabe
};

#endif
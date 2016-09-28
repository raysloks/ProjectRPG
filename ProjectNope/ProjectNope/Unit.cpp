#include "Unit.h"
#include <set>

#include "UnitCapability.h"
#include "AudioParticle.h"
#include "Matrix3.h"

#include "TestServer.h"
#include "World.h"

#include "APL.h"
#include "Decorator.h"

extern TestServer * server;

float interpolate_direction_part(float o, float target, float part)
{
	float origin = o;
	if (origin>M_PI*2.0f)
		throw std::exception();
	float diff_neg = target - M_PI*2.0f - origin;
	float diff_net = target - origin;
	float diff_pos = target + M_PI*2.0f - origin;
	if (abs(diff_net) == M_PI) {
		origin = target;
	} else {
		if (abs(diff_neg) < abs(diff_pos)) {
			if (abs(diff_net) < abs(diff_neg)) {
				origin += diff_net*part;
			} else {
				origin += diff_neg*part;
			}
		} else {
			if (abs(diff_net) < abs(diff_pos)) {
				origin += diff_net*part;
			} else {
				origin += diff_pos*part;
			}
		}
	}
	if (origin > M_PI*2) {
		origin -= M_PI*2*((int)(origin/(M_PI*2)));
	}
	if (origin < 0.0f) {
		origin += M_PI*2*(1+(int)(-origin/(M_PI*2)));
	}
	return origin;
}

float interpolate_direction_value(float o, float target, float val)
{
	float origin = o;
	if (origin>M_PI*2.0f)
		throw std::exception();
	float diff_neg = target - M_PI*2.0f - origin;
	float diff_net = target - origin;
	float diff_pos = target + M_PI*2.0f - origin;
	float final_diff = 0.0f;
	if (abs(diff_net) == M_PI) {
		origin = target;
	} else {
		if (abs(diff_neg) < abs(diff_pos)) {
			if (abs(diff_net) < abs(diff_neg)) {
				final_diff = diff_net;
			} else {
				final_diff = diff_neg;
			}
		} else {
			if (abs(diff_net) < abs(diff_pos)) {
				final_diff = diff_net;
			} else {
				final_diff = diff_pos;
			}
		}
	}
	if (abs(final_diff)<val)
		origin += final_diff;
	else
		origin += final_diff>0.0f ? val : -val;

	if (origin > M_PI*2) {
		origin -= M_PI*2*((int)(origin/(M_PI*2)));
	}
	if (origin < 0.0f) {
		origin += M_PI*2*(1+(int)(-origin/(M_PI*2)));
	}
	return origin;
}

const AutoSerialFactory<Unit> Unit::_factory_Unit("Unit");

Unit::Unit(const GlobalPosition& pos, const Vec3& vel) : Targetable(pos, vel, 1.0f), Serializable(Unit::_factory_Unit.id), decs(&ss), inventory(&ss)
{
	move_speed = 1.0f;
	turn_rate = 0.0f;
	turn_speed = 0.0f;
	mass_shift_speed = 4.0f;

	scale = 1.0f;
	m = 1.0f;
	regen_counter = 0.0f;
	time_since_landed = 1.0f;
	regen = 0.0f;

	dead = false;
}

Unit::Unit(instream& is, bool full) : Targetable(is, full), Serializable(Unit::_factory_Unit.id), decs(&ss), inventory(&ss)
{
	is >> v >> target >> facing >> health >> time_since_landed >> land_v >> cs >> scale
		>> rAnimation;

	Resource::load(rAnimation);

	dead = false;
}

Unit::~Unit(void)
{
}

void Unit::_initAPL(const std::shared_ptr<SkeletalAnimation>& anim)
{
	apl = std::make_shared<APLStatic>(std::make_shared<Pose>(anim->armature));
}

void Unit::_tickAPL(const std::shared_ptr<SkeletalAnimation>& anim, float dTime)
{
}

void Unit::writeLog(outstream& os)
{
	Entity::writeLog(os);
	os << v << target << facing << health << time_since_landed << land_v << cs;
	decs.writeLog(os);
	inventory.writeLog(os);
}

void Unit::readLog(instream& is)
{
	Entity::readLog(is);
	is >> v >> target >> facing >> health >> time_since_landed >> land_v >> cs;
	decs.readLog(is);
	inventory.readLog(is);
}

void Unit::_landed(float dTime)
{
	if (land_n==Vec3())
		return;
	float l = target.Len();
	if (l>1.0f) {
		target /= l;
		l = 1.0f;
	}

	Vec3 calc = target/l;
	calc = calc.Cross(Vec3(0.0f, 0.0f, 1.0f));
	calc = land_n.Cross(calc);
	if (target==Vec3())
		calc = target;
	calc.Normalize();
	Vec3 dif = calc*l;
	dif /= 1.0f+std::max(dif.z, 0.0f);
	/*dif *= l;*/
	dif *= move_speed;
	v -= land_v;
	//float fric = 0.001f;
	/*if (target==Vec2())
	v = Vec3();*/
	float vz = v.Dot(land_n);
	v -= land_n*vz;
	v -= dif;
	/*if ((target.Dot(v)<0.0f && v!=Vec3()) || target==Vec2())
	{
		if (v.Len()<1.0f)
			fric *= pow(v.Len(), 16);
		if (v.Len()<0.5f)
			fric *= pow(v.Len()/0.5f, 16);
		if (v.Len()<0.25f)
			fric *= pow(v.Len()/0.25f, 16);
		if (v.Len()<0.1f)
			fric *= pow(v.Len()/0.1f, 16);
	}*/
	/*fric *= 1.0f-std::min(0.2f, time_since_landed)*5.0f;
	if (fric>0.0f)
		v *= exp(log(fric)*dTime);
	else
		v *= 0.0f;
	if (v.Len()<0.05f)
		v = Vec3();*/

	float len = v.Len();

	float dynfric = 30.0f;

	if (len>7.0f)
		dynfric = 60.0f;

	dynfric *= std::min(1.0f, land_n.z*2.0f);

	Vec3 decel = v;
	if (len>dynfric*dTime) {
		decel /= len;
		decel *= dynfric*dTime;
	}

	v -= decel;
	move -= decel*0.5f*dTime;

	v += dif;
	v += land_n*vz;
	v += land_v;
}

void Unit::OnCollision(const std::shared_ptr<Collision>& col, float dTime)
{
	float fall_dmg_threshold = 10.0f;
	float fall_dmg_offset = 10.0f;
	float fall_dmg_factor_one = 5.0f;
	float fall_dmg_factor_two = 4.0f;
	
	if (col->n.z>0.2f && inc==0)
	{
		land_v = col->v;
		land_n = col->n;
		time_since_landed = 0.0f;
		
		/*mass_shift_delta = Vec3();
		mass_shift = Vec3();*/

		_landed(dTime);

		++inc;
	}

	if (col->n.Dot(v-col->v)<0.0f) {
		Vec3 a = -col->n*col->n.Dot(v-col->v);
		float vel = a.Len();

		if (vel>fall_dmg_threshold) {
			float ft = vel-fall_dmg_offset;
			health.current -= ft*fall_dmg_factor_one+ft*fall_dmg_factor_two;

			if (world->authority)
			{
				//world->AddEntity(new AudioParticle(p, v, "data/assets/audio/ouch.wav"));
			}
		}
		v += a;

		v -= col->v;

		/*float pfric = 0.9f; //air friction type
		v *= exp(log(pfric)*vel);*/

		float dynfric = 0.0f; //normal friction type

		float len = v.Len();
		Vec3 dif = v;
		if (len!=0.0f)
			dif /= len;

		dif *= std::min(std::max(0.0f, len-move_speed), vel*dynfric);

		v -= dif;

		v += col->v;
	}
}

void Unit::OnHitBy(DamageSource& src)
{
	for (auto i=onhit.begin();i!=onhit.end();) {
		if (i->expired()) {
			i = onhit.erase(i);
		} else {
			(*i->lock())(src);
			++i;
		}
	}
	health.current -= src.damage;
	if (src.damage!=0)
		cs.activate("hit");
	v += src.knockback;
}

void Unit::tick(float dTime)
{
	for (auto i=uc.begin();i!=uc.end();++i)
		(*i)->tick(dTime);

	target = Vec3(cs.input["x"], cs.input["y"], cs.input["z"]);
	if (target.Len()>1.0f)
		target.Normalize();

	if (!dead)
	{
		if (health.current<=0)
		{
			dead = true;
			cs.activate("dead");
		}
	}

	if (dead)
	{
		target = Vec3();
	}

	if (facing.x > M_PI*2) {
		facing.x -= M_PI*2*((int)(facing.x/(M_PI*2)));
	}
	if (facing.x < 0.0f) {
		facing.x += M_PI*2*(1+(int)(-facing.x/(M_PI*2)));
	}
	if (facing.y > M_PI/2.0f) {
		facing.y = M_PI/2.0f;
	}
	if (facing.y < -M_PI/2.0f) {
		facing.y = -M_PI/2.0f;
	}

	if (world->authority)
	{
		float target_facing = facing.x;
		if (look_target.id<0) {
			if (target!=Vec3()) {
				if (cs.input.find("dir_x")!=cs.input.end()) {
					target_facing = cs.input["dir_x"];
				} else {
					target_facing = -atan2(target.x, target.y);
				}
			}
		} else {
			/*PNEntity * ent = world->GetEntity(look_target);
			if (ent!=0) {
				Vec2 dif = Vec3(ent->p-p);
				dif.Normalize();
				target_facing = -atan2(dif.x, dif.y);
			} else {
				look_target.id = -1;
			}*/
		}
		facing.x = interpolate_direction_part(facing.x, target_facing, 1.0f-pow((float)M_E, log(turn_rate)*dTime));
		facing.x = interpolate_direction_value(facing.x, target_facing, turn_speed*dTime);

		float l = target.Len();
		if (l>1.0f) {
			target /= l;
			l = 1.0f;
		}

		/*if (time_since_landed>0.0f)
		{
			v -= mass_shift_delta;

			Vec3 mass_shift_add = target*mass_shift_speed*pow(2.0f, -mass_shift.Dot(target));
			mass_shift += mass_shift_add*dTime;
			mass_shift_delta = mass_shift_add;
			v += mass_shift_add;
		}*/

		float gravity = 9.8f;
		v.z -= gravity*dTime;
		move.z -= gravity*0.5f*dTime*dTime;

		std::vector<std::shared_ptr<Collision>> list;
		/*float feet_time = 0.05f;
		if (time_since_landed==0.0f)
		{
			float feet = 1.0f;
			world->SphereCast(p, p+Vec3(0.0f, 0.0f, -feet*dTime), r, list);
			std::shared_ptr<Collision> closest;
			for (auto i=list.begin();i!=list.end();++i)
			{
				if (closest==0)
					closest = *i;
				else
					if ((*i)->t<closest->t)
						closest = *i;
			}
			if (closest!=0)
			{
				p += Vec3(0.0f, 0.0f, -feet*dTime);
				p += closest->disp;
				OnCollision(closest, dTime);
			}
		}*/

		/*if (time_since_landed<=0.2f)
			snap = Vec3(0.0f, 0.0f, -r/4.0f);
		else
			snap = Vec3();

		Vec3 offset = v;
		offset.z = 0.0f;
		offset.Normalize();
		offset *= r;
		Vec3 line = Vec3(0.0f, 0.0f, -r*2.0f);
		list.clear();
		world->LineCheck(p+offset, p+offset+line, list);
		if (list.empty()) {
			snap = Vec3();
		} else {
		}*/

		time_since_landed += dTime;

		Targetable::tick(dTime);

		//if (inc==0)
			//_landed(dTime);

		list.clear();
		world->UnitCheck(p, r, list);
		for (auto i=list.begin();i!=list.end();++i)
		{
			if ((*i)->entity!=this) {
				Unit * unit = dynamic_cast<Unit*>((*i)->entity);
				Vec3 force = (*i)->n*(*i)->n.Dot(unit->v-v);
				float factor1 = unit->m/(m+unit->m);
				float factor2 = m/(m+unit->m);
				if (m>0.0f) {
					v += force*factor1;
					p += (*i)->disp*factor1;
				}
				if (unit->m>0.0f) {
					unit->v -= force*factor2;
					unit->p -= (*i)->disp*factor2;
				}

				OnCollision(*i, dTime);

				std::shared_ptr<Collision> rcol(new Collision(**i));
				rcol->entity = this;
				rcol->n = -(*i)->n;

				unit->OnCollision(rcol, dTime);
			}
		}

		inc = 0;

		/*if (land_n!=Vec3() && time_since_landed<=0.1f && inc==0)
		{
			_landed(dTime);
			++inc;
		}*/
	}

	std::shared_ptr<SkeletalAnimation> anim = Resource::get<SkeletalAnimation>(rAnimation);
	if (anim!=0)
	{
		if (apl==0)
			_initAPL(anim);
		if (apl!=0)
		{
			_tickAPL(anim, dTime);

			if (pose==0)
				pose.reset(new Pose(anim->armature));

			apl->tick(dTime, pose);

			for (auto i=decs.items.begin();i!=decs.items.end();++i)
				if (*i!=0)
					(*i)->attach(*pose);
		}
	}

	regen_counter += dTime*regen;
	if (regen_counter >= 1) {
		health.current += (int)regen_counter;
		if (health.current > health.max)
			health.current = health.max;
		regen_counter -= (int)regen_counter;
	}
}

#include "GUIObject.h"

#include "Profiler.h"

void Unit::render(const GlobalPosition& origin)
{
	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	glPushMatrix();

	Vec3 pos = p-origin;
	glTranslatef(pos.x, pos.y, pos.z);
	
	glTranslatef(0.0f, 0.0f, -r);
	glScalef(scale, scale, scale);

	glRotatef(facing.x*180.0f/M_PI, 0.0f, 0.0f, 1.0f);

	float col[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, col);

	for (auto i=decs.items.begin();i!=decs.items.end();++i)
		if (*i!=0)
			(*i)->render();

	glPopMatrix();

	QueryPerformanceCounter(&end);
	double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
	Profiler::add("unit-render", durationInSeconds);
}

void Unit::interpolate(Entity * pEntity, float fWeight)
{
	Targetable::interpolate(pEntity, fWeight);
	Unit * pUnit = dynamic_cast<Unit*>(pEntity);
	if (pUnit!=0) {
		pUnit->cs.update(cs);
		health = pUnit->health;
		target = pUnit->target;
		time_since_landed *= 1.0f-fWeight;
		time_since_landed += pUnit->time_since_landed*fWeight;
		land_v *= 1.0f-fWeight;
		land_v += pUnit->land_v*fWeight;
		facing.y += (pUnit->facing.y-facing.y)*fWeight;
		if (facing.y > M_PI/2.0f) {
			facing.y = M_PI/2.0f;
		}
		if (facing.y < -M_PI/2.0f) {
			facing.y = -M_PI/2.0f;
		}
		facing.x = interpolate_direction_part(facing.x, pUnit->facing.x, fWeight);
		decs = pUnit->decs;
		inventory = pUnit->inventory;
	}
}

void Unit::write_to(outstream& os, bool full) const
{
	Targetable::write_to(os, full);
	os << v << target << facing << health << time_since_landed << land_v << cs << scale
		<< rAnimation;
}
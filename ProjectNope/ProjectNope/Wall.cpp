#include "Wall.h"

#include "GUIObject.h"

Wall::Wall(const Vec3& vP1, const Vec3& vP2, const Vec3& vP3)
{
	p1 = vP1;
	p2 = vP2;
	p3 = vP3;

	Recalculate();
}

Wall::~Wall(void)
{
}

void Wall::Recalculate(void)
{
	n = (p2-p1).Cross(p3-p1);
	n.Normalize();

	n12 = n.Cross(p1-p2);
	n12.Normalize();

	n23 = n.Cross(p2-p3);
	n23.Normalize();

	n31 = n.Cross(p3-p1);
	n31.Normalize();

	l12 = (p1-p2).Len();
	l23 = (p2-p3).Len();
	l31 = (p3-p1).Len();
}

std::pair<Vec3, Vec3> Wall::GetAABB(void)
{
	Vec3 aabb_min = Vec3(INFINITE, INFINITE, INFINITE);
	Vec3 aabb_max = -aabb_min;

	aabb_max.x = std::fmaxf(aabb_max.x, p1.x);
	aabb_max.y = std::fmaxf(aabb_max.y, p1.y);
	aabb_max.z = std::fmaxf(aabb_max.z, p1.z);

	aabb_max.x = std::fmaxf(aabb_max.x, p2.x);
	aabb_max.y = std::fmaxf(aabb_max.y, p2.y);
	aabb_max.z = std::fmaxf(aabb_max.z, p2.z);

	aabb_max.x = std::fmaxf(aabb_max.x, p3.x);
	aabb_max.y = std::fmaxf(aabb_max.y, p3.y);
	aabb_max.z = std::fmaxf(aabb_max.z, p3.z);

	aabb_min.x = std::fminf(aabb_min.x, p1.x);
	aabb_min.y = std::fminf(aabb_min.y, p1.y);
	aabb_min.z = std::fminf(aabb_min.z, p1.z);

	aabb_min.x = std::fminf(aabb_min.x, p2.x);
	aabb_min.y = std::fminf(aabb_min.y, p2.y);
	aabb_min.z = std::fminf(aabb_min.z, p2.z);

	aabb_min.x = std::fminf(aabb_min.x, p3.x);
	aabb_min.y = std::fminf(aabb_min.y, p3.y);
	aabb_min.z = std::fminf(aabb_min.z, p3.z);

	return std::make_pair(aabb_min, aabb_max);
}

std::shared_ptr<Collision> ForceOut(const Vec3& p, const Vec3& v, float r)
{
	float tmpf = v.Len();
	if (tmpf<=r) {
		std::shared_ptr<Collision> col(new Collision());
		col->poc = p-v;
		col->n = v/tmpf;
		col->poo = col->poc+col->n*r;
		col->disp = col->n*(r-tmpf);
		col->dist = tmpf-r;
		col->t = 0.0f;
		return col;
	} else {
		return 0;
	}
}

std::shared_ptr<Collision> Wall::SphereCheck(const Vec3& vP, float r)
{
	float nd = (vP-p1).Dot(n);

	if (nd<=r && nd>0)
	{
		float d12 = (vP-p1).Dot(n12);
		if (d12<=r)
		{
			float d23 = (vP-p2).Dot(n23);
			if (d23<=r)
			{
				float d31 = (vP-p3).Dot(n31);
				if (d31<=r)
				{
					if (d12>=0)
					{
						float pa12 = (vP-p1).Dot(n.Cross(n12));
						if (pa12>=l12)
						{
							return ForceOut(vP,vP-p2,r);
						}
						if (pa12<=0)
						{
							return ForceOut(vP,vP-p1,r);
						}
						return ForceOut(vP,n*nd+n12*d12,r);
					}
					if (d23>=0)
					{
						float pa23 = (vP-p2).Dot(n.Cross(n23));
						if (pa23>=l23)
						{
							return ForceOut(vP,vP-p3,r);
						}
						if (pa23<=0)
						{
							return ForceOut(vP,vP-p2,r);
						}
						return ForceOut(vP,n*nd+n23*d23,r);
					}
					if (d31>=0)
					{
						float pa31 = (vP-p3).Dot(n.Cross(n31));
						if (pa31>=l31)
						{
							return ForceOut(vP,vP-p1,r);
						}
						if (pa31<=0)
						{
							return ForceOut(vP,vP-p3,r);
						}
						return ForceOut(vP,n*nd+n31*d31,r);
					}
					return ForceOut(vP,n*nd,r);
				}
			}
		}
	}

	return 0;
}

std::shared_ptr<Collision> Wall::LineCheck(const Vec3& sP, const Vec3& eP)
{
	if (sP != eP) {
		Vec3 ldd = eP-sP;
		float ldl = ldd.Len();
		Vec3 ldn = -ldd / ldl;

		Vec3 fn12 = ldn.Cross(p1-p2);
		if ((sP-p2).Dot(fn12)<=0.0f)
		{
			Vec3 fn23 = ldn.Cross(p2-p3);
			if ((sP-p3).Dot(fn23)<=0.0f)
			{
				Vec3 fn31 = ldn.Cross(p3-p1);
				if ((sP-p1).Dot(fn31)<=0.0f)
				{
					float zic = (n.Dot(p1)-n.Dot(sP))/(n.Dot(ldd));
					if (zic <= 1.0f && zic >= 0.0f) {
						std::shared_ptr<Collision> col(new Collision());
						col->poc = sP + ldd * zic;
						col->poo = col->poc;
						col->n = n;
						col->disp = ldd*(zic-1.0f); 
						col->dist = zic * ldl;
						col->t = zic;
						return col;
					}
				}
			}
		}
	}

	return 0;
}

std::shared_ptr<Collision> Wall::LineCheck(const Vec3& sP, const Vec3& eP, float r)
{
	if (sP != eP) {
		Vec3 ldd = eP-sP;
		float ldl = ldd.Len();
		Vec3 ldn = -ldd / ldl;

		Vec3 fn12 = ldn.Cross(p1+ldn*ldn.Dot(p2-p1)-p2);
		if ((sP-p2).Dot(fn12)<0.0f)
		{
			Vec3 fn23 = ldn.Cross(p2+ldn*ldn.Dot(p3-p2)-p3);
			if ((sP-p3).Dot(fn23)<0.0f)
			{
				Vec3 fn31 = ldn.Cross(p3+ldn*ldn.Dot(p1-p3)-p1);
				if ((sP-p1).Dot(fn31)<0.0f)
				{
					float zic = (n.Dot(p1)-n.Dot(sP))/(n.Dot(ldd));
					//float zic = (-n.x*sP.x+n.x*p1.x-n.y*sP.y+n.y*p1.y-n.z*sP.z+n.z*p1.z)/(n.x*ldd.x+n.y*ldd.y+n.z*ldd.z);
					if (zic <= 1.0f && zic >= 0.0f) {
						std::shared_ptr<Collision> col(new Collision());
						col->poc = sP + ldd * zic;
						col->n = n;
						col->poo = col->poc+col->n*r;
						col->disp = ldd*(zic-1.0f);
						col->dist = zic * ldl;
						col->t = zic;
						return col;
					}
				}
			}
		}
	}

	return 0;
}

std::shared_ptr<Collision> Wall::SphereLine(const Vec3& sP, const Vec3& eP, const Vec3& vP, float r)
{
	Vec3 l = eP-sP;
	float len = l.Len();
	if (len!=0.0f)
	{
		l /= len;
		Vec3 lp = l.Cross(l.Cross(sP-vP));
		lp /= lp.Len();
		float sqd = lp.Dot(sP-vP);
		sqd *= sqd;
		sqd = r*r - sqd;
		if (sqd > 0.0f) {
			sqd = sqrt(sqd);
			float d = -(l.Dot(sP-vP));
			float d_close = d-sqd;
			float d_far = d+sqd;
			if (d_close <= len && d_close >= 0.0f) {
				std::shared_ptr<Collision> col(new Collision());
				col->poo = sP + l * d_close;
				col->poc = col->poo;
				col->n = (col->poo-vP);
				col->n /= col->n.Len();
				col->disp = col->poo - eP;
				col->dist = d_close;
				col->t = col->dist/len;
				return col;
			}
		}
	}
	return 0;
}

std::shared_ptr<Collision> Wall::TubeLine(const Vec3& sP, const Vec3& eP, const Vec3& vP, const Vec3& dir, float r)
{
	Vec3 fsP = sP-dir*dir.Dot(sP-vP);
	Vec3 feP = eP-dir*dir.Dot(eP-vP);

	Vec3 l = feP-fsP;
	float len = l.Len();
	if (len!=0.0f)
	{
		l /= len;
		Vec3 lp = l.Cross(dir);
		float sqd = lp.Dot(fsP-vP);
		sqd *= sqd;
		sqd = r*r - sqd;
		if (sqd > 0.0f) {
			sqd = sqrt(sqd);
			float d = -(l.Dot(fsP-vP));
			float d_close = d-sqd;
			//float d_far = d+sqd;
			if (d_close <= len && d_close >= 0.0f) {
				std::shared_ptr<Collision> col(new Collision());
				col->poo = sP + (eP-sP) * d_close/len;
				col->n = fsP+l*d_close-vP;
				col->n.Normalize();
				col->poc = col->poo;
				col->disp = col->poo - eP;
				col->dist = d_close;
				col->t = col->dist/len;
				return col;
			}
		}
	}
	return 0;
}

std::shared_ptr<Collision> Wall::DiskCastLine(const Vec3& sP, const Vec3& eP, const Vec3& vP, const Vec3& dir, float cl, float r)
{
	Vec3 fsP = sP - dir*dir.Dot(sP - vP);
	Vec3 feP = eP - dir*dir.Dot(eP - vP);

	Vec3 l = feP - fsP;
	float len = l.Len();
	if (len != 0.0f)
	{
		l /= len;
		Vec3 lp = l.Cross(dir);
		float sqd = lp.Dot(fsP - vP);
		sqd *= sqd;
		sqd = r*r - sqd;
		if (sqd >= 0.0f) {
			sqd = sqrt(sqd);
			float d = -(l.Dot(fsP - vP));
			float d_close = d - sqd;
			if (d_close <= len && d_close >= 0.0f) {
				std::shared_ptr<Collision> col(new Collision());
				Vec3 poc = sP + (eP - sP) * d_close / len;
				col->dist = (poc - vP).Dot(dir);
				if (col->dist <= cl && col->dist >= 0.0f)
				{
					col->t = col->dist / cl;
					Vec3 dif = eP - sP;
					col->poo = vP + dir * col->dist;
					col->poc = poc;
					col->n = dif.Cross(dir).Cross(dif);
					col->n.Normalize();
					if (col->n.Dot(dir) > 0.0f)
						col->n = -col->n;
					return col;
				}
			}
		}
	}
	return 0;
}

std::shared_ptr<Collision> Wall::SphereCast(const Vec3& sP, const Vec3& eP, float r)
{
	std::shared_ptr<Collision> col = SphereCheck(sP, r);
	if (col!=0) {
		col->disp += sP-eP;
		return col;
	}
	if (sP != eP) {
		col = LineCheck(sP-n*r, eP-n*r, r);
		if (col!=0) {
			return col;
		}
		col = TubeLine(sP, eP, p1, (p2-p1)/l12, r);
		if (col!=0) {
			float l = Vec3(col->poc-p1).Dot((p2-p1)/l12);
			if (l>=0.0f && l<=l12) {
				col->poc -= col->n*r;
				//col->p = p1+(p2-p1)/l12*l;
				return col;
			}
		}
		col = TubeLine(sP, eP, p2, (p3-p2)/l23, r);
		if (col!=0) {
			float l = Vec3(col->poc-p2).Dot((p3-p2)/l23);
			if (l>=0.0f && l<=l23) {
				col->poc -= col->n*r;
				//col->p = p2+(p3-p2)/l23*l;
				return col;
			}
		}
		col = TubeLine(sP, eP, p3, (p1-p3)/l31, r);
		if (col!=0) {
			float l = Vec3(col->poc-p3).Dot((p1-p3)/l31);
			if (l>=0.0f && l<=l31) {
				col->poc -= col->n*r;
				//col->p = p3+(p1-p3)/l31*l;
				return col;
			}
		}
		col = SphereLine(sP, eP, p1, r);
		if (col!=0) {
			col->poc -= col->n*r;
			//col->p = p1;
			return col;
		}
		col = SphereLine(sP, eP, p2, r);
		if (col!=0) {
			col->poc -= col->n*r;
			//col->p = p2;
			return col;
		}
		col = SphereLine(sP, eP, p3, r);
		if (col!=0) {
			col->poc -= col->n*r;
			//col->p = p3;
			return col;
		}
	}
	return 0;
}

std::shared_ptr<Collision> Wall::DiskCast(const Vec3& sP, const Vec3& eP, float r)
{
	std::shared_ptr<Collision> col;

	Vec3 dif = eP - sP;
	float l = dif.Len();
	Vec3 dir = dif / l;

	Vec3 uphill_flat = n.Cross(dir).Cross(dir);
	uphill_flat -= dir*dir.Dot(uphill_flat);
	uphill_flat.Normalize();
	uphill_flat *= r;

	// face
	col = LineCheck(sP + uphill_flat, eP + uphill_flat);
	if (col != nullptr)
	{
		col->poo -= uphill_flat;
		return col;
	}

	Vec3 d1 = p1 - sP;
	Vec3 d2 = p2 - sP;
	Vec3 d3 = p3 - sP;
	float ld1 = d1.Dot(dir);
	float ld2 = d2.Dot(dir);
	float ld3 = d3.Dot(dir);
	Vec3 f1 = d1 - dir*ld1;
	Vec3 f2 = d2 - dir*ld2;
	Vec3 f3 = d3 - dir*ld3;
	float r2 = r*r;
	if (ld1 < ld2 && ld1 < ld3)
	{
		// edges
		col = DiskCastLine(p1, p2, sP, dir, l, r);
		if (col != nullptr)
			return col;
		col = DiskCastLine(p1, p3, sP, dir, l, r);
		if (col != nullptr)
			return col;
		// vertex
		if (f1.LenPwr() <= r2 && ld1 >= 0.0f && ld1 <= l)
		{
			col.reset(new Collision());
			col->t = ld1 / l;
			col->n = -dir;
			col->poo = sP + dir * ld1;
			col->poc = p1;
			return col;
		}
	}
	else if (ld2 < ld3)
	{
		// edges
		col = DiskCastLine(p2, p1, sP, dir, l, r);
		if (col != nullptr)
			return col;
		col = DiskCastLine(p2, p3, sP, dir, l, r);
		if (col != nullptr)
			return col;
		// vertex
		if (f2.LenPwr() <= r2 && ld2 >= 0.0f && ld2 <= l)
		{
			col.reset(new Collision());
			col->t = ld2 / l;
			col->n = -dir;
			col->poo = sP + dir * ld2;
			col->poc = p2;
			return col;
		}
	}
	else
	{
		// edges
		col = DiskCastLine(p3, p1, sP, dir, l, r);
		if (col != nullptr)
			return col;
		col = DiskCastLine(p3, p2, sP, dir, l, r);
		if (col != nullptr)
			return col;
		// vertex
		if (f3.LenPwr() <= r2 && ld3 >= 0.0f && ld3 <= l)
		{
			col.reset(new Collision());
			col->t = ld3 / l;
			col->n = -dir;
			col->poo = sP + dir * ld3;
			col->poc = p3;
			return col;
		}
	}
	return nullptr;
}

std::shared_ptr<Collision> Wall::LowerDisk(const Vec3 & lock, const Vec3 & center, const Vec3& axis, const Vec3 & dir, float r)
{
	// (p - lock) . (axis x (center + dir*t - lock)) = 0
	// (p - lock) . (axis x (center - lock)) + (p - lock) . (axis x dir*t) = 0
	// (p - lock) . (axis x (center - lock)) = -(p - lock) . (axis x dir*t)
	// (p - lock) . (axis x (center - lock)) = -(p - lock) . (axis x dir) * t
	// (p - lock) . (axis x (center - lock)) / -(p - lock) . (axis x dir) = t

	// (p - center - dir * dir . (p - center))^2 = r^2
	// (p - lock) . (axis x (center - lock)) / -(p - lock) . (axis x dir) = t

	// x
	// y
	// z
	// r
	// k
	// l
	// m
	// a

	// x^2 + y^2 = r^2
	// y = sqrt(r^2 - x^2)

	// 

	std::shared_ptr<Collision> col;

	Vec3 num_part = axis.Cross(center - lock);
	Vec3 denom_part = axis.Cross(dir);

	float t1 = (p1 - lock).Dot(num_part) / -(p1 - lock).Dot(denom_part);
	float t2 = (p2 - lock).Dot(num_part) / -(p2 - lock).Dot(denom_part);
	float t3 = (p3 - lock).Dot(num_part) / -(p3 - lock).Dot(denom_part);

	Vec3 d1 = p1 - center;
	Vec3 d2 = p2 - center;
	Vec3 d3 = p3 - center;
	float ld1 = d1.Dot(dir);
	float ld2 = d2.Dot(dir);
	float ld3 = d3.Dot(dir);
	Vec3 f1 = d1 - dir*ld1;
	Vec3 f2 = d2 - dir*ld2;
	Vec3 f3 = d3 - dir*ld3;
	float r2 = r*r;

	if (t1 < t2 && t1 < t3 && t1 >= 0.0f && t1 <= 1.0f)
	{
		if (f1.LenPwr() <= r2)
		{
			col.reset(new Collision());
			col->t = t1;
			col->poo = center + dir * t1;
			col->poc = p1;
			return col;
		}
	}
	else if (t2 < t3 && t2 >= 0.0f && t2 <= 1.0f)
	{
		if (f2.LenPwr() <= r2)
		{
			col.reset(new Collision());
			col->t = t2;
			col->poo = center + dir * t2;
			col->poc = p2;
			return col;
		}
	}
	else if (t3 >= 0.0f && t3 <= 1.0f)
	{
		if (f3.LenPwr() <= r2)
		{
			col.reset(new Collision());
			col->t = t3;
			col->poo = center + dir * t3;
			col->poc = p3;
			return col;
		}
	}

	return std::shared_ptr<Collision>();
}

float Wall::PlaneLineFloat(const Vec3& pN, float pD, const Vec3& sP, const Vec3& eP)
{
	return (pD - pN.Dot(sP)) / (pN.Dot(eP - sP));
}

Vec3 Wall::PlaneLineVector(const Vec3& pN, float pD, const Vec3& sP, const Vec3& eP)
{
	Vec3 ldd = eP - sP;
	return sP + ldd * (pD - pN.Dot(sP)) / (pN.Dot(ldd));
}

std::vector<Wall> ClipTriangle(const Vec3& one, const Vec3& two, const Vec3& outlier, const Vec3& pN, float pD)
{
	std::vector<Wall> ret;
	auto i31 = Wall::PlaneLineVector(pN, pD, outlier, one);
	auto i23 = Wall::PlaneLineVector(pN, pD, two, outlier);
	ret.push_back(Wall(outlier, i31, i23));
	ret.push_back(Wall(i31, one, i23));
	ret.push_back(Wall(i23, one, two));
	return ret;
}

std::vector<Wall> Wall::Clip(const Vec3& pN, float pD)
{
	float pd1 = pN.Dot(p1);
	float pd2 = pN.Dot(p2);
	float pd3 = pN.Dot(p3);

	if (pd1 < pD)
	{
		if (pd2 < pD)
		{
			if (pd3 < pD)
			{
				return std::vector<Wall>({ Wall(*this) });
			}
			else
			{
				return ClipTriangle(p1, p2, p3, pN, pD);
			}
		}
		else
		{
			if (pd3 < pD)
			{
				return ClipTriangle(p3, p1, p2, pN, pD);
			}
			else
			{
				return ClipTriangle(p2, p3, p1, pN, pD);
			}
		}
	}
	else
	{
		if (pd2 < pD)
		{
			if (pd3 < pD)
			{
				return ClipTriangle(p2, p3, p1, pN, pD);
			}
			else
			{
				return ClipTriangle(p3, p1, p2, pN, pD);
			}
		}
		else
		{
			if (pd3 < pD)
			{
				return ClipTriangle(p1, p2, p3, pN, pD);
			}
			else
			{
				return std::vector<Wall>({ Wall(*this) });
			}
		}
	}
}
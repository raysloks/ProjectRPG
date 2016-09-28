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

std::shared_ptr<Collision> TubeLine(const Vec3& sP, const Vec3& eP, const Vec3& vP, const Vec3& dir, float r)
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
			float d_far = d+sqd;
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
#include "DynamicWall.h"
#include "Vec2.h"
#include <iostream>

DynamicWall::DynamicWall(const Vec3& vP1, const Vec3& vP2, const Vec3& vP3) : Wall(vP1, vP2, vP3), vV1(), vV2(), vV3()
{
}

DynamicWall::~DynamicWall(void)
{
}

Vec3 DynamicWall::getWeight(const Vec3& p)const
{
	Vec3 nx = (p1-p3)/l31;
	Vec3 ny = nx.Cross(n);

	Vec2 rp = Vec2(Vec3(p-p3).Dot(nx), Vec3(p-p3).Dot(ny));
	Vec2 r1 = Vec2(l31, (p1-p3).Dot(ny));
	Vec2 r2 = Vec2((p2-p3).Dot(nx), (p2-p3).Dot(ny));

	float w1 = (r2.y*rp.x-r2.x*rp.y)/(r2.y*r1.x-r2.x*r1.y);
	float w2 = (-r1.y*rp.x+r1.x*rp.y)/(r2.y*r1.x-r2.x*r1.y);
	float w3 = 1.0f-w1-w2;

	if (!(w1==w1 && w2==w2 && w3==w3))
	{
		w1 = 1.0f/3.0f;
		w2 = w1;
		w3 = w2;
	}

	return Vec3(w1, w2, w3);
}

Vec3 calcWeight(const Vec3& p1, const Vec3& p2, const Vec3& p3, const Vec3& p)
{
	/*Vec3 n = (p2-p1).Cross(p3-p1);
	n.Normalize();

	Vec3 nx = (p1-p3);
	float l31 = nx.Len();
	nx /= l31;
	Vec3 ny = nx.Cross(n);

	Vec2 rp = Vec2(Vec3(p-p3).Dot(nx), Vec3(p-p3).Dot(ny));
	Vec2 r1 = Vec2(l31, (p1-p3).Dot(ny));
	Vec2 r2 = Vec2((p2-p3).Dot(nx), (p2-p3).Dot(ny));

	float w1 = (r2.y*rp.x-r2.x*rp.y)/(r2.y*r1.x-r2.x*r1.y);
	float w2 = (-r1.y*rp.x+r1.x*rp.y)/(r2.y*r1.x-r2.x*r1.y);
	float w3 = 1.0f-w1-w2;

	if (!(w1==w1 && w2==w2 && w3==w3))
	{
		w1 = 1.0f/3.0f;
		w2 = w1;
		w3 = w2;
	}

	return Vec3(w1, w2, w3);*/

	Vec3 c1 = p2-p1;
	Vec3 c2 = p3-p1;
	Vec3 point = p-p1;

	Vec3 n = c1.Cross(c2).Normalize();

	point -= n*n.Dot(point);

	float w2 = c2.Cross(point).Dot(n) / c2.Cross(c1).Dot(n);
	float w3 = c1.Cross(point).Dot(n) / c1.Cross(c2).Dot(n);
	float w1 = 1.0f-w2-w3;

	return Vec3(w1, w2, w3);
}

//test for calcWeight
//Vec3 test1, test2, test3, test4;
//
//test1 = Vec3(1.0f, 1.0f, 1.0f);
//test2 = Vec3(2.0f, 7.0f, 1.0f);
//test3 = Vec3(2.0f, 1.0f, 1.0f);
//test4 = Vec3(0.0f, 9.0f, 4.0f);
//
//Vec3 testw = calcWeight(test1, test2, test3, test4);
//
//Vec3 testf = test1*testw.x+test2*testw.y+test3*testw.z;
//
//std::cout << testf.x << " " << testf.y << " " << testf.z << std::endl;

void DynamicWall::applyVelocity(const std::shared_ptr<Collision>& col)const
{
	if (col!=0)
	{
		Vec3 w = getWeight(col->poc);

		col->v = vV1*w.x+vV2*w.y+vV3*w.z;
	}
}

bool DynamicWall::validate(const Vec3& sP, const Vec3& dif, float t)const
{
	if (t<=1.0f && t>=0.0f)
	{
		Vec3 cp1 = p1+vV1*t/60.0f;
		Vec3 cp2 = p2+vV3*t/60.0f;
		Vec3 cp3 = p3+vV3*t/60.0f;
		Vec3 cn = (cp2-cp1).Cross(cp3-cp1);
		cn.Normalize();
		if (cn.Cross(cp1-cp2).Dot(sP+dif*t-cp1)<=.0f)
		{
			if (cn.Cross(cp2-cp3).Dot(sP+dif*t-cp2)<=.0f)
			{
				if (cn.Cross(cp3-cp1).Dot(sP+dif*t-cp3)<=.0f)
				{
					return true;
				}
			}
		}
	}
	return false;
}

std::shared_ptr<Collision> DynamicWall::SphereCheck(const Vec3& vP, float r)
{
	std::shared_ptr<Collision> col = Wall::SphereCheck(vP, r);
	applyVelocity(col);
	return col;
}

std::shared_ptr<Collision> DynamicWall::LineCheck(const Vec3& sP, const Vec3& eP)
{
	std::shared_ptr<Collision> col = Wall::LineCheck(sP, eP);
	applyVelocity(col);
	return col;
}

std::shared_ptr<Collision> DynamicWall::SphereCast(const Vec3& sP, const Vec3& eP, float r)
{
	std::shared_ptr<Collision> col;
	{
		col = Wall::SphereCheck(sP, r);
		if (col!=0)
		{
			applyVelocity(col);
			if (col->n.Dot(n)>=0.0f)
				return col;
		}

		float dTime = 1.0f/144.0f;

		Vec3 dif = eP-sP;
		if (true)
		{
			Vec3 o1 = p2-p1;
			Vec3 de1 = (vV2-vV1)*dTime;
			Vec3 o2 = p3-p1;
			Vec3 de2 = (vV3-vV1)*dTime;
			Vec3 o3 = sP-p1;
			Vec3 de3 = dif-vV1*dTime;

			double rad = r;
			double radsqr = rad*rad;

			double a = o1.x;double b = o1.y;double c = o1.z;
			double d = o2.x;double e = o2.y;double f = o2.z;
			double g = o3.x;double h = o3.y;double j = o3.z;

			double k = de1.x;double l = de1.y;double m = de1.z;
			double n = de2.x;double o = de2.y;double p = de2.z;
			double q = de3.x;double r = de3.y;double s = de3.z;

			double a3 = k*o*s - k*p*r - l*n*s + l*p*q + m*n*r - m*o*q;
			double a2 = a*o*s - a*p*r - b*n*s + b*p*q + c*n*r - c*o*q - d*l*s + d*m*r + e*k*s - e*m*q - f*k*r + f*l*q + g*l*p - g*m*o - h*k*p + h*m*n + j*k*o - j*l*n;
			double a1 = a*e*s - a*f*r - a*h*p - b*d*s + b*f*q + b*g*p + c*d*r - c*e*q - c*g*o + c*h*n + d*h*m - e*g*m + f*g*l - f*h*k + a*j*o - b*j*n - d*j*l + e*j*k;
			double a0 = b*f*g - a*f*h + c*d*h - c*e*g + a*e*j - b*d*j;

			double ko_ln = (k*o - l*n);
			double kp_mn = k*p - m*n;
			double lp_mo = (l*p - m*o);
			double ao_bn_dl__ek = (a*o - b*n - d*l + e*k);
			double ap_cn_dm__fk = (a*p - c*n - d*m + f*k);
			double bp_cp_em__fl = (b*p - c*o - e*m + f*l);
			double ae_bd = a*e - b*d;
			double af_cd = a*f - c*d;
			double bf_ce = b*f - c*e;

			double b4 = ko_ln*ko_ln + kp_mn*kp_mn + lp_mo*lp_mo;
			double b3 = 2*(k*o - l*n)*(a*o - b*n - d*l + e*k) + 2*(k*p - m*n)*(a*p - c*n - d*m + f*k) + 2*(l*p - m*o)*(b*p - c*o - e*m + f*l);
			double b2 = ao_bn_dl__ek*ao_bn_dl__ek + ap_cn_dm__fk*ap_cn_dm__fk + bp_cp_em__fl*bp_cp_em__fl + 2*(a*e - b*d)*(k*o - l*n) + 2*(a*f - c*d)*(k*p - m*n) + 2*(b*f - c*e)*(l*p - m*o);
			double b1 = 2*(a*e - b*d)*(a*o - b*n - d*l + e*k) + 2*(a*f - c*d)*(a*p - c*n - d*m + f*k) + 2*(b*f - c*e)*(b*p - c*o - e*m + f*l);
			double b0 = ae_bd*ae_bd + af_cd*af_cd + bf_ce*bf_ce;

			double c6 = a3*a3;
			double c5 = 2.0*a3*a2;
			double c4 = 2.0*a3*a1 + a2*a2 - b4*radsqr;
			double c3 = 2.0*(a2*a1 + a3*a0) - b3*radsqr;
			double c2 = 2.0*a2*a0 + a1*a1 - b2*radsqr;
			double c1 = 2.0*a1*a0 - b1*radsqr;
			double c0 = a0*a0 - b0*radsqr;

			double d5 = 6.0*c6;
			double d4 = 5.0*c5;
			double d3 = 4.0*c4;
			double d2 = 3.0*c3;
			double d1 = 2.0*c2;
			double d0 = c1;

			double xn = 0.0;
			double next_try = 1.0;
			for (int i=0;i<64;++i)
			{
				double f = c0 + xn*(c1 + xn*(c2 + xn*(c3 + xn*(c4 + xn*(c5 + xn*(c6))))));
				double fp = d0 + xn*(d1 + xn*(d2 + xn*(d3 + xn*(d4 + xn*(d5)))));
				if (fp==0.0) { // found a stationary point
					if (f!=0.0)
						xn = -1.0;
				}
				if (f/fp==0.0)
					break;
				xn = xn - f/fp;
				if (xn<0.0) {
					xn = next_try;
					if (xn<0.0)
						break;
					next_try = -1.0;
				}
			}

			std::shared_ptr<Collision> ret(new Collision());

			if (xn>=0.0 && xn<=1.0)
			{
				ret->t = xn;
				Vec3 w = calcWeight(Vec3(), o1+de1*xn, o2+de2*xn, o3+de3*xn);
				if (w.x>=0.0f && w.y>=0.0f && w.z>=0.0f)
				{
					ret->n = (o1+de1*xn).Cross(o2+de2*xn).Normalize();
					if (ret->n.Dot(o3+de3*xn)>=0.0f)
					{
						ret->n = (o1+de1).Cross(o2+de2).Normalize();
						ret->poc = (p1+vV1*dTime)*w.x + (p2+vV2*dTime)*w.y + (p3+vV3*dTime)*w.z;
						ret->poo = ret->poc + ret->n*rad;

						return ret;
					}
				}
			}
		}
		if (false)
		{
			Vec3 v1 = vV1/60.0f;
			Vec3 v2 = vV2/60.0f;
			Vec3 v3 = vV3/60.0f;

			Vec3 ct2 = v2.Cross(v3)-v2.Cross(v1)-v1.Cross(v3);
			Vec3 ct1 = p2.Cross(v3)+v2.Cross(p3)-p2.Cross(v1)-v2.Cross(p1)-p1.Cross(v3)-v1.Cross(p3);
			Vec3 ct0 = p2.Cross(p3)-p2.Cross(p1)-p1.Cross(p3);
			float a = v1.Dot(ct2)-dif.Dot(ct2);
			float b = p1.Dot(ct2)+v1.Dot(ct1)-sP.Dot(ct2)-dif.Dot(ct1);
			float c = p1.Dot(ct1)+v1.Dot(ct0)-sP.Dot(ct1)-dif.Dot(ct0);
			float d = p1.Dot(ct0)-sP.Dot(ct0);
			float t = 2.0f;
			float t1 = 2.0f;
			float t2 = 2.0f;
			float t3 = 2.0f;
			if (d==0.0f)
			{
				d=c;
				c=b;
				b=a;
				a=0.0f;
			}
			if (a!=0.0f)
			{
				b /= a;
				c /= a;
				d /= a;
				float disc, q, ra, s, u, term1;
				q = (3.0f*c - (b*b))/9.0f;
				ra = -27.0f*d + b*(9.0f*c - 2.0f*b*b);
				ra /= 54.0f;
				disc = q*q*q + ra*ra;
				term1 = b/3.0f;
				if (disc > 0.0f) {
					s = ra + sqrt(disc);
					s = s < 0.0f ? -pow(-s, 1.0f/3.0f) : pow(s, 1.0f/3.0f);
					u = ra - sqrt(disc);
					u = u < 0.0f ? -pow(-u, 1.0f/3.0f) : pow(u, 1.0f/3.0f);
					t1 = -term1 + s + u;
				}
				if (disc == 0.0f) {
					u = ra < 0.0f ? -pow(-ra, 1.0f/3.0f) : pow(ra, 1.0f/3.0f);
					t1 = -term1 + 2.0f*u;
					t2 = -(u+term1);
					t3 = t2;
				}
				if (disc < 0.0f) {
					q = -q;
					s = q*q*q;
					s = acos(ra/sqrt(s));
					u = 2.0f*sqrt(q);
					t1 = -term1 + u*cos(s/3.0f);
					t2 = -term1 + u*cos((s + 2.0f*M_PI)/3.0f);
					t3 = -term1 + u*cos((s + 4.0f*M_PI)/3.0f);
				}

				if (validate(sP, dif, t1))
					t = t1;
				if (t2 < t && validate(sP, dif, t2))
					t = t2;
				if (t3 < t && validate(sP, dif, t3))
					t = t3;
			}
			else
			{
				if (b!=0.0f)
				{
					c /= b;
					d /= b;

					float q = c*c/4-d;
					if (q>=0.0f)
					{
						float t1 = -c/2.0f-sqrt(q);
						if (validate(sP, dif, t1))
							t = t1;
						float t2 = -c/2.0f+sqrt(q);
						if (t2 < t && validate(sP, dif, t2))
							t = t2;
					}
				}
				else
				{
					if (c!=0.0f) {
						t = -d/c;
						if (!validate(sP, dif, t))
							t = 2.0f;
					}
				}
			}
			
			if (t<=1.0f && t>=0.0f)
			{
				std::cout << t << std::endl;

				Vec3 cp1 = p1+v1*t;
				Vec3 cp2 = p2+v2*t;
				Vec3 cp3 = p3+v3*t;
				Vec3 cn = (cp2-cp1).Cross(cp3-cp1);
				cn.Normalize();

				col.reset(new Collision());
				col->poc = sP+dif*t;
				Vec3 w = calcWeight(cp1, cp2, cp3, col->poc); // this does another calculation of cn. optimize!
				std::cout << w.x << " , " << w.y << " , " << w.z << std::endl;
				col->poc += v1*w.x*(1.0f-t);
				col->poc += v2*w.y*(1.0f-t);
				col->poc += v3*w.z*(1.0f-t);

				cp1 += v1*(1.0f-t);
				cp2 += v2*(1.0f-t);
				cp3 += v3*(1.0f-t);
				cn = (cp2-cp1).Cross(cp3-cp1);
				cn.Normalize();

				col->poo = col->poc+cn*r;
				col->n = cn;
				col->t = t;
				col->disp = col->poo-eP;
				col->dist = Vec3(col->poo-sP).Len();
				col->ce = this;
				col->v = vV1*w.x+vV2*w.y+vV3*w.z;
				return col;
			}
		}
	}
	//if (vV1==Vec3() && vV2==Vec3() && vV3==Vec3())
	if (false)
	{
		col = Wall::SphereCast(sP, eP, r);
		if (col!=0)
		{
			/*col->poc += med*col->t;
			col->poo += med*col->t;*/
			applyVelocity(col);
			if (col->n.Dot(n)<0.0f)
				return 0;
			return col;
		}
	}
	return 0;
}
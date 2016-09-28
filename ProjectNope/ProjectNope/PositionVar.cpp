#include "PositionVar.h"
#include "FloatVar.h"
#include "BooleanVar.h"
#include "StringVar.h"

PositionVar::PositionVar(void)
{
}

PositionVar::PositionVar(const GlobalPosition& pos) : p(pos)
{
}

PositionVar::~PositionVar(void)
{
}

Variable * PositionVar::clone(void) const
{
	return new PositionVar(p);
}

std::shared_ptr<Variable> PositionVar::operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate)
{
	std::shared_ptr<PositionVar> vp = std::dynamic_pointer_cast<PositionVar>(rhs);
	if (vp!=0)
	{
		switch(op.front())
		{
		case'+':
			if (op.back()=='=') {
				p += vp->p;
				return *std::shared_ptr<std::shared_ptr<Variable>>(address);
			} else {
				return std::shared_ptr<Variable>(new PositionVar(p+vp->p));
			}
			break;
		case'-':
			if (op.back()=='=') {
				p -= vp->p;
				return *std::shared_ptr<std::shared_ptr<Variable>>(address);
			} else {
				return std::shared_ptr<Variable>(new PositionVar(p-vp->p));
			}
			break;
		/*case'*': //TODO dot product? cross product?
			if (op.back()=='=') {
				f *= vf->f;
				return *std::shared_ptr<std::shared_ptr<Variable>>(address);
			} else {
				return std::shared_ptr<Variable>(new PositionVar(f*vf->f));
			}
		case'/':
			if (op.back()=='=') {
				f /= vf->f;
				return *std::shared_ptr<std::shared_ptr<Variable>>(address);
			} else {
				return std::shared_ptr<Variable>(new PositionVar(f/vf->f));
			}*/
		/*case'<':
			if (op.back()=='=')
				return std::shared_ptr<Variable>(new BooleanVar(Vec3(p).LenPwr()<=Vec3(vp->p).LenPwr()));
			else
				return std::shared_ptr<Variable>(new BooleanVar(Vec3(p).LenPwr()<Vec3(vp->p).LenPwr()));
		case'>':
			if (op.back()=='=')
				return std::shared_ptr<Variable>(new BooleanVar(Vec3(p).LenPwr()>=Vec3(vp->p).LenPwr()));
			else
				return std::shared_ptr<Variable>(new BooleanVar(Vec3(p).LenPwr()>Vec3(vp->p).LenPwr()));*/
		case'=':
			if (op.back()=='=' && op.size()==2) {
				return std::shared_ptr<Variable>(new BooleanVar(Vec3(p)==Vec3(vp->p)));
			} else {
				p=vp->p;
				return *std::shared_ptr<std::shared_ptr<Variable>>(address);
			}
			break;
		case'!':
			if (op.back()=='=') {
				return std::shared_ptr<Variable>(new BooleanVar(Vec3(p)!=Vec3(vp->p)));
			}
			break;
		default:
			break;
		}
	}
	std::shared_ptr<FloatVar> vf = std::dynamic_pointer_cast<FloatVar>(rhs);
	if (vf!=0)
	{
		switch(op.front())
		{
		/*case'+':
			if (op.back()=='=') {
				p += vp->p;
				return *std::shared_ptr<std::shared_ptr<Variable>>(address);
			} else {
				return std::shared_ptr<Variable>(new PositionVar(p+vp->p));
			}
		case'-':
			if (op.back()=='=') {
				p -= vp->p;
				return *std::shared_ptr<std::shared_ptr<Variable>>(address);
			} else {
				return std::shared_ptr<Variable>(new PositionVar(p-vp->p));
			}*/
		case'*':
			if (op.back()=='=') {
				p = Vec3(p)*vf->f;
				return *std::shared_ptr<std::shared_ptr<Variable>>(address);
			} else {
				return std::shared_ptr<Variable>(new PositionVar(Vec3(p)*vf->f));
			}
			break;
		case'/':
			if (op.back()=='=') {
				p = Vec3(p)/vf->f;
				return *std::shared_ptr<std::shared_ptr<Variable>>(address);
			} else {
				return std::shared_ptr<Variable>(new PositionVar(Vec3(p)/vf->f));
			}
			break;
		/*case'<':
			if (op.back()=='=')
				return std::shared_ptr<Variable>(new BooleanVar(Vec3(p).LenPwr()<=vf->f*vf->f));
			else
				return std::shared_ptr<Variable>(new BooleanVar(Vec3(p).LenPwr()<vf->f*vf->f));
		case'>':
			if (op.back()=='=')
				return std::shared_ptr<Variable>(new BooleanVar(Vec3(p).LenPwr()>=vf->f*vf->f));
			else
				return std::shared_ptr<Variable>(new BooleanVar(Vec3(p).LenPwr()>vf->f*vf->f));
		case'=':
			if (op.back()=='=' && op.size()==2) {
				return std::shared_ptr<Variable>(new BooleanVar(Vec3(p).LenPwr()==vf->f*vf->f));
			} else {
				p=vp->p;
				return *std::shared_ptr<std::shared_ptr<Variable>>(address);
			}
		case'!':
			if (op.back()=='=') {
				return std::shared_ptr<Variable>(new BooleanVar(Vec3(p)!=Vec3(vp->p)));
			}*/
		default:
			break;
		}
	}
	std::shared_ptr<StringVar> vstr = std::dynamic_pointer_cast<StringVar>(rhs);
	if (vstr!=0)
	{
		switch(op.front())
		{
		case'.':
			if (vstr->str.compare("x")==0)
				return std::shared_ptr<FloatVar>(new FloatVar(Vec3(p).x));
			if (vstr->str.compare("y")==0)
				return std::shared_ptr<FloatVar>(new FloatVar(Vec3(p).y));
			if (vstr->str.compare("z")==0)
				return std::shared_ptr<FloatVar>(new FloatVar(Vec3(p).z));
			if (vstr->str.compare("l")==0)
				return std::shared_ptr<FloatVar>(new FloatVar(Vec3(p).Len()));
			break;
		default:
			break;
		}
	}
	if (rhs==0)
	{
		if (op.compare("-")==0)
			return std::shared_ptr<Variable>(new PositionVar(GlobalPosition()-p));
	}
	if (allocate)
		return Variable::operate(op, rhs);
	else
		return 0;
}
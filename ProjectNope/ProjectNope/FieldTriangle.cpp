#include "FieldTriangle.h"

#include <map>

FieldTriangle::FieldTriangle(void)
{
}

FieldTriangle::FieldTriangle(const Vec3& a, const Vec3& b, const Vec3& c) : p1(a), p2(b), p3(c)
{
}

FieldTriangle::~FieldTriangle(void)
{
}

FieldTriangle& FieldTriangle::order_cw(Vec3 n)
{
	if (n.Dot((p2-p1).Cross(p3-p1))<0.0f) {
		Vec3 temp = p2;
		p2 = p3;
		p3 = p2;
	}
	return *this;
}

FieldTriangle& FieldTriangle::order_ccw(Vec3 n)
{
	if (n.Dot((p3-p1).Cross(p2-p1))<0.0f) {
		Vec3 temp = p2;
		p2 = p3;
		p3 = p2;
	}
	return *this;
}

SplitPair FieldTriangle::split(Vec3 n, float l)
{
	SplitPair ret;

	float d1 = n.Dot(p1);
	float d2 = n.Dot(p2);
	float d3 = n.Dot(p3);

	std::multimap<float, Vec3> flat;
	auto first = flat.insert(std::make_pair(d1, p1));
	auto second = flat.insert(std::make_pair(d2, p2));
	auto last = flat.insert(std::make_pair(d3, p3));

	auto i = flat.lower_bound(l);
	switch(std::distance(flat.begin(), i))
	{
	case 0:
		ret.second.push_back(FieldTriangle(*this));
		break;
	case 1:
		switch(flat.count(l))
		{
		case 0:
			{
				Vec3 split1 = (second->second-first->second).Normalize();
				float n1 = split1.Dot(n)*(l-first->first);
				split1 = split1*n1+first->second;
				Vec3 split2 = (last->second-first->second).Normalize();
				float n2 = split2.Dot(n)*(l-first->first);
				split2 = split2*n2+first->second;
				ret.first.push_back(FieldTriangle(first->second, split1, split2));
				ret.second.push_back(FieldTriangle(split1, last->second, split2));
				ret.second.push_back(FieldTriangle(split1, second->second, last->second));
			}
			break;
		case 1:
			{
				Vec3 split = (last->second-first->second).Normalize();
				float n2 = split.Dot(n)*(l-first->first);
				split = split*n2+first->second;
				ret.first.push_back(FieldTriangle(first->second, second->second, split));
				ret.second.push_back(FieldTriangle(split, second->second, last->second));
			}
			break;
		case 2:
			ret.first.push_back(FieldTriangle(*this));
		}
		break;
	case 2:
		switch(flat.count(l))
		{
		case 0:
			{
				Vec3 split1 = (second->second-last->second).Normalize();
				float n1 = split1.Dot(n)*(l-last->first);
				split1 = split1*n1+last->second;
				Vec3 split2 = (first->second-last->second).Normalize();
				float n2 = split2.Dot(n)*(l-last->first);
				split2 = split2*n2+last->second;
				ret.second.push_back(FieldTriangle(last->second, split2, split1));
				ret.first.push_back(FieldTriangle(split2, first->second, split1));
				ret.first.push_back(FieldTriangle(split1, first->second, second->second));
			}
			break;
		case 1:
			ret.first.push_back(FieldTriangle(*this));
		}
		break;
	case 3:
		ret.first.push_back(FieldTriangle(*this));
	}

	return ret;
}

#include <iostream>

MergeTuple FieldTriangle::merge(const FieldTriangle& other)
{
	Vec3 n12 = ((p2-p1).Cross(p1)).Normalize();
	Vec3 n23 = ((p3-p2).Cross(p2)).Normalize();
	Vec3 n31 = ((p1-p3).Cross(p3)).Normalize();

	std::vector<Vec3> this_contains;

	if (other.p1.Dot(n12)<=0.0f && other.p1.Dot(n23)<=0.0f && other.p1.Dot(n31)<=0.0f)
	{
		this_contains.push_back(other.p1);
	}
	if (other.p2.Dot(n12)<=0.0f && other.p2.Dot(n23)<=0.0f && other.p2.Dot(n31)<=0.0f)
	{
		this_contains.push_back(other.p2);
	}
	if (other.p3.Dot(n12)<=0.0f && other.p3.Dot(n23)<=0.0f && other.p3.Dot(n31)<=0.0f)
	{
		this_contains.push_back(other.p3);
	}

	Vec3 on12 = ((other.p2-other.p1).Cross(other.p1)).Normalize();
	Vec3 on23 = ((other.p3-other.p2).Cross(other.p2)).Normalize();
	Vec3 on31 = ((other.p1-other.p3).Cross(other.p3)).Normalize();

	std::vector<Vec3> other_contains;

	if (p1.Dot(on12)<=0.0f && p1.Dot(on23)<=0.0f && p1.Dot(on31)<=0.0f)
	{
		other_contains.push_back(p1);
	}
	if (p2.Dot(on12)<=0.0f && p2.Dot(on23)<=0.0f && p2.Dot(on31)<=0.0f)
	{
		other_contains.push_back(p2);
	}
	if (p3.Dot(on12)<=0.0f && p3.Dot(on23)<=0.0f && p3.Dot(on31)<=0.0f)
	{
		other_contains.push_back(p3);
	}

	switch(this_contains.size())
	{
	case 0:
		switch(other_contains.size())
		{
		}
		break;
	case 1:
		switch(other_contains.size())
		{
		}
		break;
	case 2:
		switch(other_contains.size())
		{
		}
		break;
	case 3:
		switch(other_contains.size())
		{
		}
		break;
	}

	std::cout << "this contains " << this_contains.size() << " of other points." << std::endl;
	std::cout << "other contains " << other_contains.size() << " of this points." << std::endl;

	return MergeTuple();
}
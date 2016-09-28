#include "ControlState.h"

ControlState::ControlState(void) : empty(true)
{
}

ControlState::~ControlState(void)
{
}

#include <iostream>
#include <algorithm>

void ControlState::activate(const std::string& str)
{
	if (check[str]++==INT_MAX)
		check[str]=INT_MIN;
	++active[str];
}

void ControlState::update(ControlState& cs)
{
	for (auto i=input.begin();i!=input.end();++i)
		cs.input[i->first]=i->second;
	for (auto i=check.begin();i!=check.end();++i)
	{
		auto it = cs.check.find(i->first);
		if (it!=cs.check.end())
		{
			int difference = i->second<INT_MIN / 2 && it->second>INT_MAX / 2 ? it->second + i->second + 2 : it->second - i->second;
			if (difference > 0)
				cs.active[i->first] += difference;
			it->second = i->second;
		}
		else
		{
			if (!cs.empty)
				cs.active[i->first] = i->second;
			cs.check[i->first] = i->second;
		}
	}
	cs.empty = false;
}

outstream& operator<<(outstream& os, const ControlState& cs)
{
	os << (uint32_t)cs.input.size();
	for (auto i=cs.input.begin();i!=cs.input.end();++i)
	{
		os << i->first << i->second;
	}
	os << (uint32_t)cs.check.size();
	for (auto i=cs.check.begin();i!=cs.check.end();++i)
	{
		os << i->first << i->second;
	}
	return os;
}

instream& operator>>(instream& is, ControlState& cs)
{
	uint32_t l;
	std::string str;
	float f;
	is >> l;
	for (int i=0;i<l;++i)
	{
		is >> str >> f;
		cs.input[str] = f;
	}
	int val;
	is >> l;
	for (int i=0;i<l;++i)
	{
		is >> str >> val;
		cs.check[str] = val;
	}
	return is;
}
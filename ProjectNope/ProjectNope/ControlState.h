#include <map>
#include <set>
#include <string>
#include "streams.h"

#ifndef CONTROL_STATE_H
#define CONTROL_STATE_H

class ControlState
{
public:
	ControlState(void);
	~ControlState(void);

	void activate(const std::string& str);
	void update(ControlState& cs);

	std::map<std::string, float> input;
	std::map<std::string, int> check;
	std::map<std::string, int> active;

	bool empty;
};

outstream& operator<<(outstream& os, const ControlState& cs);
instream& operator>>(instream& is, ControlState& cs);

#endif
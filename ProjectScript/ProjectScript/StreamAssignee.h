#ifndef STREAM_ASSIGNEE_H
#define STREAM_ASSIGNEE_H

#include "ScriptCompile.h"

template <typename T>
class StreamAssignee
{
public:
	StreamAssignee(std::ostream& stream) : s(stream) {}

	StreamAssignee<T>& operator=(const T& rhs)
	{
		s.write((char*)&rhs, sizeof(T));
		return *this;
	}

	std::ostream& s;
};

template <typename T>
class StreamAssigneeRelative;

template <>
class StreamAssigneeRelative<int8_t>
{
public:
	StreamAssigneeRelative(ScriptCompile& comp) : c(comp), stream(comp.ss) {}

	StreamAssigneeRelative<int8_t>& operator=(const int8_t& rhs)
	{
		c.rel8.push_back(stream.tellp());
		stream.write((char*)&rhs, sizeof(int8_t));
		return *this;
	}

	ScriptCompile& c;
	std::ostream& stream;
};

template <>
class StreamAssigneeRelative<int32_t>
{
public:
	StreamAssigneeRelative(ScriptCompile& comp) : c(comp), stream(comp.ss) {}

	StreamAssigneeRelative<int32_t>& operator=(const int32_t& rhs)
	{
		c.rel32.push_back(stream.tellp());
		stream.write((char*)&rhs, sizeof(int32_t));
		return *this;
	}

	ScriptCompile& c;
	std::ostream& stream;
};

#endif
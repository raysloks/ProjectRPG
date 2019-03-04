#ifndef STREAM_ASSIGNEE_H
#define STREAM_ASSIGNEE_H

#include "ScriptCompile.h"

template <typename T>
class StreamAssignee
{
public:
	StreamAssignee(ScriptCompile& comp) : c(comp), stream(comp.ss) {}

	StreamAssignee<T>& operator=(const T& rhs)
	{
		/*if (!c.proto)
			throw std::runtime_error("Attempting to generate code outside a function.");*/
		stream.write((char*)&rhs, sizeof(T));
		return *this;
	}

	ScriptCompile& c;
	std::ostream& stream;
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
		/*if (!c.proto)
			throw std::runtime_error("Attempting to generate code outside a function.");*/
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
		/*if (!c.proto)
			throw std::runtime_error("Attempting to generate code outside a function.");*/
		c.rel32.push_back(stream.tellp());
		stream.write((char*)&rhs, sizeof(int32_t));
		return *this;
	}

	ScriptCompile& c;
	std::ostream& stream;
};

#endif
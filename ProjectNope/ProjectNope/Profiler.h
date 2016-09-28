#ifndef PROFILER_H
#define PROFILER_H

#include <string>

class Profiler
{
public:
	Profiler(void);
	~Profiler(void);

	static void add(const std::string& name, double time);
	static void print(void);
	static std::string get(void);
};

#endif
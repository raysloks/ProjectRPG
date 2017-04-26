#ifndef PROFILER_H
#define PROFILER_H

#include <string>

class Timeslot
{
public:
	Timeslot(const std::string& slot);
	~Timeslot(void);
};

class Profiler
{
public:
	Profiler(void);
	~Profiler(void);

	static void start(const std::string& slot);
	static void stop(void);
	static void reset(void);
	static void set(const std::string& slot, double value);
	static std::string get(void);
};

#endif
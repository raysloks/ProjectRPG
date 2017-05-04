#ifndef PROFILER_H
#define PROFILER_H

#include <string>

#define TIMESLOT_LEVEL 3

#define TimeslotA(x)
#define TimeslotB(x)
#define TimeslotC(x)

#if TIMESLOT_LEVEL >= 1
#undef TimeslotA(x)
#define TimeslotA(x) Timeslot timeslot_ ## x ( #x )
#endif
#if TIMESLOT_LEVEL >= 2
#undef TimeslotB(x)
#define TimeslotB(x) Timeslot timeslot_ ## x ( #x )
#endif
#if TIMESLOT_LEVEL >= 3
#undef TimeslotC(x)
#define TimeslotC(x) Timeslot timeslot_ ## x ( #x )
#endif

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
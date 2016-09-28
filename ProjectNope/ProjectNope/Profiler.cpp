#include "Profiler.h"

Profiler::Profiler(void)
{
}

Profiler::~Profiler(void)
{
}

#include <map>
#include <boost\thread.hpp>

std::map<std::string, double> map;
boost::mutex profiler_mutex;

void Profiler::add(const std::string& name, double time)
{
	boost::lock_guard<boost::mutex> lock(profiler_mutex);
	map[name] += time;
}

#include <iostream>

void Profiler::print(void)
{
	boost::lock_guard<boost::mutex> lock(profiler_mutex);
	for (auto i=map.begin();i!=map.end();++i)
		std::cout << i->first << ": " <<i->second << std::endl;
	map.clear();
}

std::string Profiler::get(void)
{
	boost::lock_guard<boost::mutex> lock(profiler_mutex);
	if (map.size()>0) {
		std::string ret = map.begin()->first + ": " + std::to_string((long double)map.begin()->second);
		map.erase(map.begin());
		return ret;
	} else {
		return std::string();
	}
}
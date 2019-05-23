#include "Profiler.h"

#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <cmath>

class TimeslotData
{
public:
	void reset(void)
	{
		//time = 0.0;
		for each (auto slot in map)
		{
			slot.second->reset();
		}
	}

	std::string get(size_t tabs)
	{
		std::string str;
		for each (auto slot in map)
		{
			for (size_t i = 0; i < tabs; i++)
				str.append("    ");
			str.append(slot.first + " : " + std::to_string((int)std::round(slot.second->time)) + "\n");
			str.append(slot.second->get(tabs + 1));
		}
		return str;
	}

	std::map<std::string, std::shared_ptr<TimeslotData>> map;
	double time;
	std::chrono::high_resolution_clock::time_point start;
};

std::shared_ptr<TimeslotData> timeslot(new TimeslotData());
std::vector<std::string> slot_path;

std::mutex profiler_mutex;

std::string output;

Profiler::Profiler(void)
{
}

Profiler::~Profiler(void)
{
}

void Profiler::start(const std::string& slot)
{
	std::lock_guard<std::mutex> lock(profiler_mutex);
	std::shared_ptr<TimeslotData> current_timeslot = timeslot;
	for each (auto path in slot_path)
	{
		current_timeslot = current_timeslot->map[path];
	}
	if (current_timeslot->map.find(slot) == current_timeslot->map.end())
		current_timeslot->map.insert(std::make_pair(slot, std::make_shared<TimeslotData>()));
	current_timeslot->map[slot]->start = std::chrono::high_resolution_clock::now();
	slot_path.push_back(slot);
}

void Profiler::stop(void)
{
	std::lock_guard<std::mutex> lock(profiler_mutex);
	std::shared_ptr<TimeslotData> current_timeslot = timeslot;
	for each (auto path in slot_path)
	{
		current_timeslot = current_timeslot->map[path];
	}
	current_timeslot->time += std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - current_timeslot->start).count();
	slot_path.pop_back();
}

void Profiler::reset(void)
{
	std::lock_guard<std::mutex> lock(profiler_mutex);
	output = timeslot->get(0);
	//timeslot.map.clear();
	timeslot->reset();
}

void Profiler::set(const std::string& slot, double value)
{
	std::lock_guard<std::mutex> lock(profiler_mutex);
	std::shared_ptr<TimeslotData> current_timeslot = timeslot;
	for each (auto path in slot_path)
	{
		current_timeslot = current_timeslot->map[path];
	}
	if (current_timeslot->map.find(slot) == current_timeslot->map.end())
		current_timeslot->map.insert(std::make_pair(slot, std::make_shared<TimeslotData>()));
	current_timeslot->map[slot]->time = value;
}

std::string Profiler::get(void)
{
	std::lock_guard<std::mutex> lock(profiler_mutex);
	return output;
}

Timeslot::Timeslot(const std::string& slot)
{
	Profiler::start(slot);
}

Timeslot::~Timeslot(void)
{
	Profiler::stop();
}
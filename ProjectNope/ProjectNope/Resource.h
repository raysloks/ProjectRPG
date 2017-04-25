#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>

#include <set>

#include <thread>

class Resource
{
public:
	Resource(void);
	virtual ~Resource(void);

	template <class T>
	static std::shared_ptr<T> get(const std::string& name, const std::set<std::string>& options = std::set<std::string>()) {
		return std::dynamic_pointer_cast<T>(load(name, options));
	}

	static void add(const std::string& name, const std::shared_ptr<Resource>& res);
	static std::shared_ptr<Resource> load(const std::string& name, const std::set<std::string>& options = std::set<std::string>());
	static bool is_loaded(const std::string& name);
	//void add(const std::string& name, void * resource);
	static void unload(const std::string& name);
	static void unload(void);
};

#endif
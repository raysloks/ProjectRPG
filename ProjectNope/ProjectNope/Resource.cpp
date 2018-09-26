#include "Resource.h"
#include <fstream>
#include "TGA.h"
#include "Sound.h"
#include "Mesh.h"
#include "SkeletalAnimation.h"
#include "StringResource.h"
#include "FontResource.h"

#include "ShaderProgram.h"
#include "streams.h"

#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <sstream>

std::unordered_map<std::string, std::shared_ptr<Resource>> resources;
std::unordered_map<std::string, std::pair<std::thread, bool>> loading;
std::mutex mutex;

Resource::Resource(void)
{
}

Resource::~Resource(void)
{
}

void _load(std::string name, std::set<std::string> options)
{
	bool blocking = options.find("block") != options.end();

	try
	{
		//LARGE_INTEGER freq, start, end;
		//QueryPerformanceFrequency(&freq);
		//QueryPerformanceCounter(&start);

		//std::cout << std::string("loading resource: ") + name << std::endl;
		std::fstream f(name, std::ios_base::binary | std::ios_base::in);
		if (!f.is_open() || f.bad() || !f.good()) {
			mutex.lock();
			if (!blocking)
				loading[name].second = false;
			resources.insert(std::pair<std::string, std::shared_ptr<Resource>>(name, nullptr));
			mutex.unlock();
			//f.close();
			//std::cout << std::string("failed to load resource: ") + name + "\r\n";
			return;
		}
		std::stringstream buffer;
		buffer << f.rdbuf();

		//f.close();

		//QueryPerformanceCounter(&end);
		//double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;

		//std::cout << std::string("file closed: ") + name << " taking " << durationInSeconds << " seconds" << std::endl;

		std::istringstream is(buffer.str());

		std::pair<std::string, std::shared_ptr<Resource>> res;
		if (name.find(".tga") != std::string::npos) {
			res = std::pair<std::string, std::shared_ptr<Resource>>(name, std::shared_ptr<Resource>(new TGA(instream(is.rdbuf()), options)));
		}
		else if (name.find(".wav") != std::string::npos) {
			res = std::pair<std::string, std::shared_ptr<Resource>>(name, std::shared_ptr<Resource>(new Sound(instream(is.rdbuf()), options)));
		}
		else if (name.find(".gmdl") != std::string::npos) {
			res = std::pair<std::string, std::shared_ptr<Resource>>(name, std::shared_ptr<Resource>(new Mesh(instream(is.rdbuf()))));
		}
		else if (name.find(".anim") != std::string::npos) {
			res = std::pair<std::string, std::shared_ptr<Resource>>(name, std::shared_ptr<Resource>(new SkeletalAnimation(instream(is.rdbuf()))));
		}
		else if (name.find(".ttf") != std::string::npos) {
			res = std::pair<std::string, std::shared_ptr<Resource>>(name, std::shared_ptr<Resource>(new FontResource(buffer.str())));
		}
		else {
			std::shared_ptr<StringResource> text(new StringResource(buffer.str()));
			res = std::pair<std::string, std::shared_ptr<Resource>>(name, text);
		}

		mutex.lock();
		if (!blocking)
			loading[name].second = false;
		resources.insert(res);
		mutex.unlock();

		//QueryPerformanceCounter(&end);
		//durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;

		//std::cout << std::string("resource finalized: ") + name << " taking " << durationInSeconds << " seconds" << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << "Failed to load resource \"" << name << "\". Reason: \"" << e.what() << '"' << std::endl;

		mutex.lock();
		if (!blocking)
			loading[name].second = false;
		resources.insert(std::pair<std::string, std::shared_ptr<Resource>>(name, nullptr));
		mutex.unlock();
	}
}

void Resource::add(const std::string& name, const std::shared_ptr<Resource>& res)
{
	mutex.lock();
	resources[name] = res;
	mutex.unlock();
}

std::shared_ptr<Resource> Resource::load(const std::string& name, const std::set<std::string>& options)
{
	bool blocking = options.find("block") != options.end();

	std::shared_ptr<std::lock_guard<std::mutex>> lock(new std::lock_guard<std::mutex>(mutex));
	if (loading.find(name) == loading.cend())
	{

		auto it = resources.find(name);
		if (it!=resources.end()) {
			if (options.find("reload")!=options.end()) {
				resources.erase(it);
			} else {
				if (it->second != nullptr)
					return resources[name];
				else
					return nullptr;
			}
		}
;
		lock.reset();
		if (blocking)
		{
			_load(name, options);
			mutex.lock();
			auto resource = resources[name];
			mutex.unlock();
			return resource;
		}
		else
		{
			mutex.lock();
			loading[name] = std::make_pair(std::thread(std::bind(&_load, name, options)), true);
			mutex.unlock();
			return nullptr;
		}

	}

	if (blocking || !loading[name].second)
	{
		loading[name].first.join();
		loading.erase(name);
		return resources[name];
	}
	else
	{
		return nullptr;
	}
}

void Resource::unload(const std::string& name)
{
	mutex.lock();
	resources.erase(name);
	mutex.unlock();
}

void Resource::unload(void)
{
	mutex.lock();
	resources.clear();
	for (auto i = loading.begin(); i != loading.end(); ++i)
	{
		i->second.first.join();
	}
	loading.clear();
	mutex.unlock();
}

bool Resource::is_loaded(const std::string& name)
{
	mutex.lock();
	bool found = resources.find(name)!=resources.end();//loading.find(name)!=loading.end();
	mutex.unlock();
	return found;
}
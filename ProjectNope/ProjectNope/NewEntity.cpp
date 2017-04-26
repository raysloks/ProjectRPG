#include "NewEntity.h"

#include "Component.h"

#include <sstream>

#include <iostream>

#include "Profiler.h"

NewEntity::NewEntity(void)
{
}

NewEntity::NewEntity(instream& is, bool full)
{
	uint32_t n;
	is >> n;
	for (size_t i = 0; i < n; i++)
	{
		if (!full)
		{
			int sync;
			is >> sync;
			component_sync.push_back(sync);
		}
		auto factory = Serializable::unserialize(is);
		Component * comp;
		if (factory != nullptr)
			comp = dynamic_cast<Component*>(factory->create(is, full));
		else
			comp = nullptr;
		addComponent(comp, full);
	}
}

NewEntity::~NewEntity(void)
{
	//maybe call disconnect on all components?
	for (auto i = components.begin(); i != components.end(); ++i)
		if (*i != nullptr)
			delete *i;
	for (auto i = component_syncref.begin(); i != component_syncref.end(); ++i)
		ss.deallocate(*i);
}

std::vector<Component*>::iterator NewEntity::addComponent(Component * pComponent, bool authority)
{
	components.push_back(pComponent);
	if (pComponent != nullptr)
	{
		pComponent->connect(this, authority);
		pComponent->entity = this;
	}
	const size_t id = component_syncref.size();
	if (authority)
		component_syncref.push_back(ss.allocate([this, id] (ClientData&) {
			conf.insert(id);
		}, [pComponent] (ClientData& client) -> bool {
			return pComponent->visible(client);
		}));
	return components.end()-1;
}

std::vector<Component*>::iterator NewEntity::removeComponent(Component * pComponent)
{
	for (auto i = components.begin();i != components.end(); ++i)
	{
		if (*i == pComponent)
		{
			(*i)->disconnect();
			delete *i;
			*i = nullptr;
			return i;
		}	
	}
	return components.end();
}

void NewEntity::pre_frame(float dTime)
{
	for (auto i = components.begin(); i != components.end(); ++i)
	{
		if (*i != nullptr)
		{
			Timeslot timeslot_component(Serializable::getName((*i)->getSerialID()));
			(*i)->pre_frame(dTime);
		}
	}
}

void NewEntity::post_frame(float dTime)
{
	for (auto i = components.begin(); i != components.end(); ++i)
	{
		if (*i != nullptr)
		{
			Timeslot timeslot_component(Serializable::getName((*i)->getSerialID()));
			(*i)->post_frame(dTime);
		}
	}
}

void NewEntity::tick(float dTime)
{
	for (auto i = components.begin(); i != components.end(); ++i)
	{
		if (*i != nullptr)
		{
			Timeslot timeslot_component(Serializable::getName((*i)->getSerialID()));
			(*i)->tick(dTime);
		}
	}
}

void NewEntity::writeLog(outstream& os, ClientData& client)
{
	std::stringbuf logs_buf;
	outstream logs(&logs_buf);
	for (auto i = components.begin(); i != components.end(); ++i)
	{
		if (*i != nullptr)
		{
			if ((*i)->visible(client))
			{
				std::stringbuf cbuf;
				outstream comp(&cbuf);
				(*i)->writeLog(comp, client);
				if (cbuf.str().size())
				{
					logs << (uint32_t)std::distance(components.begin(), i); //maybe use 8 or 16 bits instead
					logs.write(cbuf.str().data(), cbuf.str().size());
				}
			}
		}
	}

	if (logs_buf.str().size() || conf.size())
	{
		for (auto i = conf.begin(); i != conf.end(); ++i)
		{
			os << uint32_t(*i) << ss.sync[component_syncref[std::distance(conf.begin(), i)]];
			Serializable::serialize(os, components[*i]);
			if (components[*i] != nullptr)
				components[*i]->write_to(os, client);
		}
		conf.clear();
		os << (uint32_t)0xffffffff;
		os.write(logs_buf.str().data(), logs_buf.str().size());
		os << (uint32_t)0xffffffff;
	}
}

void NewEntity::readLog(instream& is)
{
	while (!is.eof())
	{
		uint32_t index, sync;
		is >> index;
		if (index != 0xffffffff)
		{
			is >> sync;
			auto factory = Serializable::unserialize(is);
			Component * comp;
			if (factory != nullptr)
				comp = dynamic_cast<Component*>(factory->create(is, false));
			else
				comp = nullptr;
			if (components.size() <= index)
			{
				components.resize(index);
				components.push_back(comp);
				component_sync.resize(index);
				component_sync.push_back(sync);
			}
			else
			{
				if (SyncState::is_ordered(component_sync[index], sync))
				{
					if (components[index] != nullptr)
					{
						components[index]->disconnect();
						delete components[index];
					}
					component_sync[index] = sync;
					components[index] = comp;
					if (comp != nullptr)
					{
						comp->connect(this, false);
						comp->entity = this;
					}
				}
				else
				{
					if (comp != nullptr)
						delete comp;
				}
			}
		}
		else
			break;
	}

	while (!is.eof())
	{
		uint32_t index;
		is >> index;
		if (index != 0xffffffff)
		{
			if (index < components.size())
				if (components[index] != nullptr)
					components[index]->readLog(is);
		}
		else
			break;
	}
}

void NewEntity::writeLog(outstream& os)
{
	bool has_written = false;
	for (auto i = components.begin(); i != components.end(); ++i)
	{
		if (*i != nullptr)
		{
			std::stringbuf cbuf;
			outstream comp(&cbuf);
			(*i)->writeLog(comp);
			if (cbuf.str().size())
			{
				has_written = true;
				os << (uint32_t)std::distance(components.begin(), i);
				os.write(cbuf.str().data(), cbuf.str().size());
				//std::cout << "write log " << Serializable::getName((*i)->getSerialID()) << std::endl;
			}
		}
	}
	if (has_written)
		os << uint32_t(0xffffffff);
}

void NewEntity::readLog(instream& is, ClientData& client)
{
	while (!is.eof())
	{
		uint32_t index;
		is >> index;
		if (index != 0xffffffff)
		{
			if (index < components.size())
			{
				if (components[index] != nullptr)
				{
					//std::cout << "read log " << Serializable::getName(components[index]->getSerialID()) << std::endl;
					components[index]->readLog(is, client);
				}
			}
		}
		else
		{
			break;
		}
	}
}

void NewEntity::interpolate(NewEntity * pEntity, float fWeight)
{
	for (auto i = components.begin(); i != components.end(); ++i)
		if (*i != nullptr)
			(*i)->interpolate(*(pEntity->components.begin()+std::distance(components.begin(), i)), fWeight);
}

void NewEntity::write_to(outstream& os, ClientData& client) const
{
	os << (uint32_t)components.size();
	for (auto i = components.begin(); i != components.end(); ++i) {
		os << ss.sync[component_syncref[std::distance(components.begin(), i)]];
		if (*i != nullptr)
			if ((*i)->visible(client))
			{
				os << (*i)->getSerialID();
				(*i)->write_to(os, client);
			}
			else
				os << uint32_t(0);
		else
			os << uint32_t(0);
	}
}

void NewEntity::write_to(outstream& os) const
{
	os << (uint32_t)components.size();
	for (auto i=components.begin();i!=components.end();++i) {
		if (*i != nullptr) {
			os << (*i)->getSerialID();
			(*i)->write_to(os);
		} else
			os << uint32_t(0);
	}
}

Component * NewEntity::getComponent(size_t type) const
{
	for (auto i=components.begin();i!=components.end();++i)
		if (*i != nullptr)
			if ((*i)->getSerialID()==type)
				return *i;
	return nullptr;
}

std::vector<Component*> NewEntity::getComponents(size_t type) const
{
	std::vector<Component*> ret;
	for (auto i=components.begin();i!=components.end();++i)
		if (*i != nullptr)
			if ((*i)->getSerialID()==type)
				ret.push_back(*i);
	return ret;
}
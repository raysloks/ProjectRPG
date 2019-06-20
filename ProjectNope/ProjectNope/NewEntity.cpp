#include "NewEntity.h"

#include "Component.h"

#include "World.h"

#include <sstream>

#include <iostream>

#include "Profiler.h"

NewEntity::NewEntity(void)
{
}

NewEntity::NewEntity(instream& is)
{
	uint32_t n;
	is >> n;
	for (size_t i = 0; i < n; i++)
	{
		//if (!full)
		{
			int sync;
			is >> sync;
			component_sync.push_back(sync);
		}
		auto factory = Component::_registry.deserialize(is);
		Component * comp;
		if (factory != nullptr)
			comp = dynamic_cast<Component*>(factory->create(is));
		else
			comp = nullptr;
		addComponent(comp);
	}
}

NewEntity::~NewEntity(void)
{
	for (auto i = components.begin(); i != components.end(); ++i)
	{
		if (*i != nullptr)
		{
			(*i)->disconnect();
			delete *i;
		}
	}

	for (auto i = component_syncref.begin(); i != component_syncref.end(); ++i)
	{
		ss.deallocate(*i);
	}
}

EntityID NewEntity::get_id(void) const
{
	return EntityID(id, world->uid[id]);
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
	{
		component_syncref.push_back(ss.allocate([this, id](ClientData&) {
			conf.insert(id);
		}, [pComponent](ClientData& client) -> bool {
			return pComponent->visible(client);
		}));
	}
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
#if TIMESLOT_LEVEL == 4
			Timeslot timeslot_component(Serializable::getName((*i)->getSerialID()));
#endif
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
#if TIMESLOT_LEVEL == 4
			Timeslot timeslot_component(Serializable::getName((*i)->getSerialID()));
#endif
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
#if TIMESLOT_LEVEL == 4
			Timeslot timeslot_component(Serializable::getName((*i)->getSerialID()));
#endif
			(*i)->tick(dTime);
		}
	}
}

void NewEntity::writeLog(outstream& os, const std::shared_ptr<ClientData>& client)
{
	std::stringbuf logs_buf;
	outstream logs(&logs_buf);
	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i] != nullptr)
		{
			if (components[i]->visible(*client))
			{
				std::stringbuf cbuf;
				outstream comp(&cbuf);
				components[i]->writeLog(comp, client);
				if (cbuf.str().size())
				{
					logs << (uint32_t)i;
					logs.write(cbuf.str().data(), cbuf.str().size());
				}
			}
		}
	}

	if (logs_buf.str().size() || conf.size())
	{
		for (auto i = conf.begin(); i != conf.end(); ++i)
		{
			os << (uint32_t)*i << ss.sync[component_syncref[std::distance(conf.begin(), i)]];
			if (components[*i])
			{
				os << components[*i]->_serial_id;
				components[*i]->write_to(os, client);
			}
			else
			{
				os << uint32_t(0);
			}
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
			auto factory = Component::_registry.deserialize(is);
			Component * comp;
			if (factory != nullptr)
				comp = factory->create(is, false);
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
				if (SyncState::is_ordered_strict(component_sync[index], sync))
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

void NewEntity::readLog(instream& is, const std::shared_ptr<ClientData>& client)
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

void NewEntity::write_to(outstream& os, const std::shared_ptr<ClientData>& client) const
{
	os << (uint32_t)components.size();
	for (auto i = components.begin(); i != components.end(); ++i) {
		os << ss.sync[component_syncref[std::distance(components.begin(), i)]];
		if (*i != nullptr)
			if ((*i)->visible(*client))
			{
				os << (*i)->_serial_id;
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
			os << (*i)->_serial_id;
			(*i)->write_to(os);
		} else
			os << uint32_t(0);
	}
}

Component * NewEntity::getComponent(uint32_t type) const
{
	for (auto i=components.begin();i!=components.end();++i)
		if (*i != nullptr)
			if ((*i)->_serial_id==type)
				return *i;
	return nullptr;
}

std::vector<Component*> NewEntity::getComponents(uint32_t type) const
{
	std::vector<Component*> ret;
	for (auto i=components.begin();i!=components.end();++i)
		if (*i != nullptr)
			if ((*i)->_serial_id==type)
				ret.push_back(*i);
	return ret;
}
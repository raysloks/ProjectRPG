#include "Chunk.h"
#include <fstream>
#include "World.h"

#include "NewEntity.h"

#include "Resource.h"
#include "StringResource.h"

#include <streams.h>

Chunk::Chunk(World * w, const std::vector<int>& p) : world(w), pos(p)
{
	loading = false;
}

Chunk::Chunk(World * w, const std::vector<int>& p, const std::string& fn) : world(w), pos(p), fname(fn)
{
	loading = true;
	Resource::load(fname);
}

Chunk::~Chunk(void)
{
}

void Chunk::sync(void)
{
	if (loading)
	{
		loading = !Resource::is_loaded(fname);

		auto data = Resource::get<StringResource>(fname);
		if (data!=0)
		{
			std::stringstream iss(data->string);
			instream is(iss.rdbuf());
			uint32_t total_units;
			is >> total_units;
			//std::cout << total_units << " entities found." << std::endl;
			NewEntity * ent;
			std::vector<NewEntity*> ents;
			int id;
			bool next;
			for (int i=0;i<total_units;++i)
			{
				is >> next;
				if (next)
				{
					//std::cout << "parsing entity " << i << "..." << std::endl;
					ent = new NewEntity(is, true);
					ents.push_back(ent);
				}
				else
				{
					ents.push_back(0);
				}
			}
			for (int i=0;i<ents.size();++i)
			{
				if (ents[i]!=0)
				{
					id = world->AddEntity(ents[i]);
					units.push_back(id);
				}
				else
				{
					alloc.push(units.size());
					units.push_back(-1);
				}
			}
		}

		if (!loading)
			Resource::unload(fname);
	}
}

size_t Chunk::add(int id)
{
	NewEntity * ent = world->GetEntity(id);
	size_t cid;
	if (alloc.size()>0) {
		cid = alloc.top();
		units[cid] = id;
		alloc.pop();
	} else {
		cid = units.size();
		units.push_back(id);
	}
	if (ent!=0) {
		/*ent->chunk = this;
		ent->cid = cid;*/
	}
	return cid;
}

void Chunk::remove(size_t id)
{
	units[id] = -1;
	alloc.push(id);
}

void Chunk::transfer(size_t id, Chunk * chunk)
{
	NewEntity * ent = world->GetEntity(units[id]);
	chunk->add(units[id]);
	/*for (auto i=ent->links.link.begin();i!=ent->links.link.end();++i)
	{
		if (*i!=0)
		{
			Chunk * pChunk = world->chunks[(*i)->chunk];
			NewEntity * other = world->GetEntity(pChunk->units[pChunk->link.oref[(*i)->lid].first]);
			std::shared_ptr<Link> other_link = other->links.link[pChunk->link.oref[(*i)->lid].second];
			other_link->chunk = chunk->pos;
			link.remove(other_link->lid);
			other_link->lid = chunk->link.add(std::make_pair(ent->cid, i-ent->links.link.begin()));
		}
	}*/
	remove(id);
}

void Chunk::unload(void)
{
	for (auto i=units.begin();i!=units.end();++i)
	{
		if (*i>=0)//SetEntity can actually handle -1 ids, but just for clarity's sake
			world->SetEntity(*i, 0);
	}
	units.clear();
}

void Chunk::save(const std::string& folder) //TODO disable/delay saving to after the chunk has been fully loaded
{
	std::string fname = folder + 
		std::to_string((_Longlong)pos[0]) + std::string("_") + 
		std::to_string((_Longlong)pos[1]) + std::string("_") + 
		std::to_string((_Longlong)pos[2]) + std::string(".dat");
	std::cout << "saving chunk " << fname << std::endl;
	std::fstream f;
	f.open(fname, std::ios::out | std::ios::binary);
	outstream os(f.rdbuf());
	os << (uint32_t)units.size();
	for (auto i=units.begin();i!=units.end();++i)
	{
		std::cout << "saving entity " << *i << std::endl;
		NewEntity * ent = world->GetEntity(*i);
		if (ent!=0) {
			os << true;
			ent->write_to(os);
		} else {
			os << false;
		}
	}
	f.close();
}
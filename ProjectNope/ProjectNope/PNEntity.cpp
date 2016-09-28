#include "PNEntity.h"
#include "World.h"

const AutoSerialFactory<PNEntity> PNEntity::_factory_PNEntity("PNEntity");

PNEntity::PNEntity(const GlobalPosition& pos) : Serializable(Entity::_factory_Entity.id), Entity(pos)
{
	chunk = 0;
	range = 1000000.0f;
}

PNEntity::PNEntity(instream& is, bool full) : Serializable(Entity::_factory_Entity.id), Entity(is, full)
{
	chunk = 0;
	range = 1000000.0f;
}

PNEntity::~PNEntity(void)
{
}

PNEntity * PNEntity::follow(size_t id)
{
	std::shared_ptr<Link> pLink = links.link[id];
	if (pLink!=0) {
		Chunk * pChunk = world->chunks[pLink->chunk];
		if (pChunk!=0)
			return 0;//world->GetEntity(pChunk->units[pChunk->link.oref[pLink->lid].first]);
	}
	return 0;
}

size_t PNEntity::link(PNEntity * other)
{
	std::shared_ptr<Link> this_link(new Link());
	std::shared_ptr<Link> other_link(new Link());
	size_t this_lid = links.add(this_link);
	size_t other_lid = other->links.add(other_link);

	other_link->chunk = chunk->pos;
	other_link->lid = chunk->link.add(std::make_pair(cid, this_lid));
	this_link->chunk = other->chunk->pos;
	this_link->lid = other->chunk->link.add(std::make_pair(other->cid, other_lid));

	return this_lid;
}
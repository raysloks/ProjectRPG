#include <memory>
#include "EntityLinkContainer.h"

EntityLinkContainer::EntityLinkContainer(void)
{
}

EntityLinkContainer::~EntityLinkContainer(void)
{
}

size_t EntityLinkContainer::add(const std::shared_ptr<Link>& ref)
{
	size_t lid;
	if (alloc.size()>0) {
		lid = alloc.top();
		link[lid] = ref;
		alloc.pop();
	} else {
		lid = link.size();
		link.push_back(ref);
	}
	return lid;
}

void EntityLinkContainer::remove(size_t lid)
{
	alloc.push(lid);
}
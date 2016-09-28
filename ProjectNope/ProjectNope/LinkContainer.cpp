#include "LinkContainer.h"

LinkContainer::LinkContainer(void)
{
}

LinkContainer::~LinkContainer(void)
{
}

size_t LinkContainer::add(const std::pair<size_t, size_t>& ref)
{
	size_t lid;
	if (alloc.size()>0) {
		lid = alloc.top();
		oref[lid] = ref;
		alloc.pop();
	} else {
		lid = oref.size();
		oref.push_back(ref);
	}
	return lid;
}

void LinkContainer::remove(size_t lid)
{
	alloc.push(lid);
}
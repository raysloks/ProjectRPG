#include "Synchronizer.h"

#include "BlendUtility.h"

Synchronizer::Synchronizer(float l)
{
}

Synchronizer::~Synchronizer(void)
{
}

void Synchronizer::blend(Synchronizer& other)
{
	for (size_t i = 0; i < other.val.size() && i < var.size(); i++)
	{
		*var[i].second = other.val[i];
	}
	other.val.clear();
}

void Synchronizer::writeLog(outstream& os) const
{
	os << (uint32_t)var.size();
	for (auto i = var.begin(); i != var.end(); ++i)
		os << *i->second;
}

void Synchronizer::readLog(instream& is)
{
	uint32_t size;
	is >> size;
	val.resize(size);
	for (size_t i = 0; i < size; i++)
		is >> val[i];
}
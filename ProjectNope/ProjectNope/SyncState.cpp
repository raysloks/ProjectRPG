#include "SyncState.h"

SyncState::SyncState(void)
{
}

SyncState::~SyncState(void)
{
}

size_t SyncState::allocate(std::function<void(ClientData&)> f, std::function<bool(ClientData&)> c)
{
	size_t rval;
	if (empty.size()>0)
	{
		rval = empty.top();
		empty.pop();
		func[rval] = f;
		condition[rval] = c;
	}
	else
	{
		rval = sync.size();
		sync.push_back(0);
		func.push_back(f);
		condition.push_back(c);
		pass.resize(sync.size());
	}
	update(rval);
	return rval;
}

void SyncState::deallocate(size_t index)
{
	update(index);
	empty.push(index);
}

void SyncState::update(size_t index)
{
	increment(sync[index]);
	conf.insert(std::make_pair(index, sync[index]));
}

void SyncState::set(size_t index, int value)
{
	if (index>=sync.size())
		sync.resize(index+1);
	sync[index] = value;
}


void SyncState::prep(const std::map<size_t, int>& map, ClientData& client_data)
{
	npass = 0;
	for (auto i=map.cbegin();i!=map.cend();++i)
	{
		if (i->first<condition.size() && i->first<func.size()) {
			bool passed = true;
			if (condition[i->first]!=0)
				passed = condition[i->first](client_data);
			if (passed) {
				if (func[i->first]!=0)
					func[i->first](client_data);
				++npass;
			}
			pass[i->first]=passed;
		}
	}
}

void SyncState::increment(int& i)
{
	if (i==INT_MAX)
		i=INT_MIN;
	else
		++i;
}

bool SyncState::is_ordered(const int& first, const int& second)
{
	return first<second || (second<INT_MIN/2 && first>INT_MAX/2);
}
#include "InventoryDisplay.h"
#include <algorithm>
#include <locale>

InventoryDisplay::InventoryDisplay(void)
{
}

InventoryDisplay::~InventoryDisplay(void)
{
}

void InventoryDisplay::update(Inventory * const inventory)
{
	if (inventory!=last || sync!=inventory->cs)
	{
		display_list.clear();
		display_list.reserve(inventory->items.size());
		for (auto i=inventory->items.begin();i!=inventory->items.end();++i)
		{
			if (*i!=0)
				display_list.push_back(*i);
		}

		std::stable_sort(display_list.begin(), display_list.end(), [](const std::shared_ptr<Item>& a, const std::shared_ptr<Item>& b)->bool
		{
			std::string as = a->name;
			std::string bs = b->name;
			std::locale loc;
			for (size_t i=0;i<as.size() && i<bs.size();++i)
			{
				if (std::toupper(as[i], loc)<std::toupper(bs[i], loc))
					return true;
				if (std::toupper(as[i], loc)>std::toupper(bs[i], loc))
					return false;
				if (as[i]<bs[i])
					return true;
				if (as[i]>bs[i])
					return false;
			}
			return false;
		});
	}
}
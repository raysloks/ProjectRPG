#ifndef INVENTORY_COMPONENT_H
#define INVENTORY_COMPONENT_H

#include "Component.h"

#include "SyncContainer.h"

#include <memory>
#include <functional>

class RenderSetup;
class Item;

class InventoryComponent :
	public Component
{
public:
	InventoryComponent(void);
	InventoryComponent(instream& is, bool full);
	~InventoryComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, ClientData& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	static const AutoSerialFactory<InventoryComponent> _factory;

	void set_display(bool visible);

	std::shared_ptr<std::function<void(RenderSetup&)>> func;

	SyncContainer<Item> items;
};

#endif

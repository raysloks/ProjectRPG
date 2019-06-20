#pragma once

#include "InventoryCommand.h"

#include "EntityID.h"

class InventoryCommandInteract :
	public InventoryCommand
{
public:
	InventoryCommandInteract(const EntityID& id);
	InventoryCommandInteract(instream& is);
	~InventoryCommandInteract();

	void execute(InventoryComponent& inv);

	void write_to(outstream& os) const;

	uint32_t getSerial() const;

	static AutoSerialFactory<InventoryCommandInteract, InventoryCommand> _factory;

	EntityID entity_id;
};
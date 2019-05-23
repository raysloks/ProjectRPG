#pragma once

#include "InventoryCommand.h"

class InventoryCommandEquip :
	public InventoryCommand
{
public:
	InventoryCommandEquip(uint32_t index);
	InventoryCommandEquip(instream& is, bool full);
	~InventoryCommandEquip();

	void execute(InventoryComponent& inv);

	void write_to(outstream& os) const;

	uint32_t getSerial() const;

	static AutoSerialFactory<InventoryCommandEquip, InventoryCommand> _factory;

	uint32_t index;
};
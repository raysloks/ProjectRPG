#pragma once

#include "AutoSerialFactory.h"
#include "SerializableRegistry.h"

class InventoryComponent;

class InventoryCommand
{
public:
	InventoryCommand();
	virtual ~InventoryCommand() = 0;

	virtual void execute(InventoryComponent& inv) = 0;

	virtual void write_to(outstream& os) const = 0;

	virtual uint32_t getSerial() const = 0;

	static SerializableRegistry<InventoryCommand> _registry;
};
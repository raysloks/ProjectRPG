#pragma once

#include "Component.h"

#include "SyncContainer.h"
#include "SyncQueue.h"

#include "EntityID.h"

#include "EquipmentData.h"

#include <memory>
#include <functional>

#include "SerializableWrapper.h"

#include "InventoryCommand.h"

class RenderSetup;
class Item;
class Decorator;
class Window;
class RectangleWindow;
class TextWindow;

class InventoryComponent :
	public Component
{
public:
	InventoryComponent(void);
	InventoryComponent(instream& is);
	~InventoryComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void pre_frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, const std::shared_ptr<ClientData>& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, const std::shared_ptr<ClientData>& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, const std::shared_ptr<ClientData>& client) const;
	void write_to(outstream& os) const;

	bool visible(ClientData& client) const;

	static AutoSerialFactory<InventoryComponent, Component> _factory;

	void set_display(bool enable);

	std::weak_ptr<ClientData> clientData;

	std::shared_ptr<std::function<void(RenderSetup&)>> func;

	bool owner;
	std::string name;

	SyncContainer<Item> items;
	SyncQueue<Item> notifications;
	SyncQueue<int32_t> hp_changes;

	SyncQueue<SerializableWrapper<InventoryCommand>> commands;
	EntityID current_interact;

	EquipmentData equipment;

	std::vector<std::pair<float, Item>> notifications_display;
	std::vector<std::pair<float, std::string>> combat_text_display;

	std::shared_ptr<Window> window, abilities_window, auras_window;
	std::vector<RectangleWindow*> aura_windows;
	std::vector<TextWindow*> aura_duration_windows;
};
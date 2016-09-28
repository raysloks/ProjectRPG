#include "InventoryWindow.h"
#include "ClientData.h"
#include "Client.h"
#include "World.h"
#include "Writing.h"
#include "PlatformEvents.h"

InventoryWindow::InventoryWindow(World * pWorld, Client * pClient) : RectangleWindow(0, 0, 0, 0), world(pWorld), client(pClient)
{
	state = 0;
}

InventoryWindow::~InventoryWindow(void)
{
}

void InventoryWindow::render(void)
{
	if (state==0)
	{

		if (client->clientData!=0) {
			NewEntity * ent = world->GetEntity(client->clientData->getUnit(client->clientData->unit_id));
			if (ent!=0) {
				//Unit * unit = 0;//dynamic_cast<Unit*>(ent);
				//if (unit!=0) {
				//	disp.update(&unit->inventory);

				//	Writing::setColor(1.0f, 1.0f, 1.0f, 1.0f);

				//	glPushMatrix();
				//	glTranslatef(100.0f, 13.0f, 0.0f);
				//	for (auto i=disp.display_list.begin();i!=disp.display_list.end();++i)
				//	{
				//		(*i)->render();
				//	}
				//	glPopMatrix();
				//}
			}
		}

		client->hideCursor = false;

	}
}

bool InventoryWindow::handleEvent(IEvent * pEvent)
{
	if (state==1)
		return false;
	if (pEvent->GetType()==KeyDownEvent::event_type) {
		KeyDownEvent * ev = (KeyDownEvent*)pEvent;
		if (ev->key==Platform::KeyEvent::ESC)
		{
			state = 1;
			return true;
		}
	}
	return true;
}
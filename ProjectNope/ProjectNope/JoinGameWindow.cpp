#include "JoinGameWindow.h"

#include "PlatformEvents.h"
#include "Client.h"

#include "RectangleButtonWindow.h"

JoinGameWindow::JoinGameWindow(World * pWorld, Client * pClient, int x, int y, int width, int height) : world(pWorld), client(pClient), RectangleWindow(x, y, width, height)
{
	RectangleButtonWindow * join_button = new RectangleButtonWindow(64, 128, 128, 32);
	join_button->onClick = [this](void) {
	};
	join_button->text = "Join";

	addChild(join_button);
}

JoinGameWindow::~JoinGameWindow(void)
{
}

void JoinGameWindow::render(RenderSetup& rs)
{
	if (state == 1)
		RectangleWindow::render(rs);
}

bool JoinGameWindow::handleEvent(IEvent * pEvent)
{
	if (state == 1)
		return RectangleWindow::handleEvent(pEvent);
	return false;
}

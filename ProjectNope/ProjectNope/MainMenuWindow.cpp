#include "MainMenuWindow.h"

#include "Client.h"
#include "Server.h"

#include "World.h"

#include "ClientData.h"

#include "Resource.h"
#include "Mesh.h"

#include "GUIObject.h"

#include "RectangleButtonWindow.h"
#include "JoinGameWindow.h"

MainMenuWindow::MainMenuWindow(World * pWorld, Client * pClient, int px, int py, int width, int height) : RectangleWindow(px, py, width, height), world(pWorld), client(pClient)
{
	time = 0.0f;
	time_move = 0.0f;
	state = 2;

	JoinGameWindow * join_game_window = new JoinGameWindow(world, client, 256, 256, 256, 256);

	RectangleButtonWindow * start_game_button = new RectangleButtonWindow(64, 256, 256, 32);
	start_game_button->onClick = [this] (void) {
		if (client->world->authority)
			client->connect("127.0.0.1", 7777);
		this->state = 2;
	};
	start_game_button->onRender = [this, start_game_button] (void) {
		if (client->world->authority)
			start_game_button->text = "New Game";
		else
			start_game_button->text = "Resume Game";
	};

	RectangleButtonWindow * join_game_button = new RectangleButtonWindow(64, 256+64, 256, 32);
	join_game_button->onClick = [this, join_game_window] (void) {
		join_game_window->state ^= 1;
	};
	join_game_button->text = "Join Game";

	RectangleButtonWindow * exit_game_button = new RectangleButtonWindow(64, 256+128, 256, 32);
	exit_game_button->onClick = [this] (void) {
		if (client->con)
		{
			client->disconnect();
		}
		else
		{
			client->isAlive = false;
		}
	};
	exit_game_button->onRender = [this, exit_game_button] (void) {
		if (client->con)
		{
			exit_game_button->text = "Disconnect";
		}
		else
		{
			exit_game_button->text = "Exit Game";
		}
	};

	addChild(join_game_window);
	addChild(start_game_button);
	addChild(join_game_button);
	addChild(exit_game_button);
}

MainMenuWindow::~MainMenuWindow(void)
{
}

void MainMenuWindow::render(RenderSetup& rs)
{
	client->hideCursor = state == 2;
	if (state == 1)
		RectangleWindow::render(rs);
}

#include "PlatformEvents.h"

bool MainMenuWindow::handleEvent(IEvent * pEvent)
{
	if (state==0)
	{
		if (pEvent->GetType()==KeyDownEvent::event_type) {
			KeyDownEvent * ev = (KeyDownEvent*)pEvent;
			if (ev->key==Platform::KeyEvent::ESC)
			{
				client->isAlive = false;
				return true;
			}
			state = 1;
			time_move = 0.01f;
		}
		return false;
	}
	if (state==1)
	{
		if (pEvent->GetType()==KeyDownEvent::event_type) {
			KeyDownEvent * ev = (KeyDownEvent*)pEvent;
			if (ev->key==Platform::KeyEvent::ESC)
			{
				if (client->clientData!=0)
				{
					client->isActive = true;
					state = 2;
					//state = 0;
					//time_move = -0.01f;
					return true;
				}
			}
		}
		if (pEvent->GetType() == MouseMoveEvent::event_type) {
			MouseMoveEvent * ev = (MouseMoveEvent*)pEvent;
			if (ev->relative)
				return true;
		}
		return RectangleWindow::handleEvent(pEvent);
	}
	if (state==2)
	{
		if (pEvent->GetType()==KeyDownEvent::event_type) {
			KeyDownEvent * ev = (KeyDownEvent*)pEvent;
			if (ev->key==Platform::KeyEvent::ESC)
			{
				state = 1;
				client->isActive = false;
				return true;
			}
		}
		return false;
	}
	return false;
}
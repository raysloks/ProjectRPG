#include "MainMenuWindow.h"

#include "Client.h"
#include "Server.h"

#include "World.h"

#include "ClientData.h"

#include "Resource.h"
#include "Mesh.h"

#include "GUIObject.h"

#include "RectangleButtonWindow.h"

MainMenuWindow::MainMenuWindow(World * pWorld, Client * pClient, int px, int py, int width, int height) : RectangleWindow(px, py, width, height), world(pWorld), client(pClient)
{
	time = 0.0f;
	time_move = 0.0f;
	state = 2;

	RectangleButtonWindow * start_game_button = new RectangleButtonWindow(64, 256, 200, 32);
	start_game_button->onClick = [this, start_game_button] (void) {
		if (client->clientData==0) {
			client->clientData = new ClientData();
			client->clientData->networked = false;
		}
		this->state = 2;
	};
	start_game_button->onRender = [this, start_game_button] (void) {
		if (client->clientData==0)
			start_game_button->text = "New Game";
		else
			start_game_button->text = "Resume Game";
	};

	RectangleButtonWindow * join_game_button = new RectangleButtonWindow(64, 256+64, 200, 32);
	join_game_button->onClick = [this] (void) {
		//open join game menu
	};
	join_game_button->text = "Join Game";

	RectangleButtonWindow * exit_game_button = new RectangleButtonWindow(64, 256+128, 200, 32);
	exit_game_button->onClick = [this] (void) {
		if (client->con != nullptr)
		{
			client->disconnect();
			client->world->clear(false);
		}
		else
		{
			if (client->clientData != nullptr)
			{
				client->disconnect();
				client->world->clear(false);
				for (auto i = client->server->conns.begin(); i != client->server->conns.end(); ++i)
				{
					std::shared_ptr<ClientConnection> conn = i->second;
					if (conn->data != nullptr)
						client->server->onClientDisconnect(*conn->data);
				}
				client->server->conns.clear();
				client->server->con.reset();
			}
			else
				client->isAlive = false;
		}
	};
	exit_game_button->onRender = [this, exit_game_button] (void) {
		if (client->con!=0)
		{
			exit_game_button->text = "Disconnect";
		}
		else
		{
			exit_game_button->text = "Exit To Windows";
			if (client->clientData!=0)
				exit_game_button->text = "Exit Game";
		}
	};

	addChild(start_game_button);
	addChild(join_game_button);
	addChild(exit_game_button);
}

MainMenuWindow::~MainMenuWindow(void)
{
}

void MainMenuWindow::render(void)
{
	/*time += time_move;
	
	if (time>2.0f)
		time=2.0f;
	if (time>1.0f && state!=2)
		time=1.0f;
	if (time<0.0f)
		time=0.0f;

	float aspect = ((float)w)/((float)h);

	auto mesh = Resource::get<Mesh>("data/assets/logo.gmdl");

	if (state==1)
		RectangleWindow::render();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);*/

	if (state!=2)
	{
		client->hideCursor = false;

		//glMatrixMode(GL_PROJECTION);
		//glPushMatrix();
		//glLoadIdentity();
		//gluPerspective(45.0, aspect, 0.1, 10.0);

		//glMatrixMode(GL_MODELVIEW);
		//glPushMatrix();
		//glLoadIdentity();
		//glTranslatef(0.0f, 0.0f, -5.0f);
		//glTranslatef(0.0f, time*1.5f, 0.0f);
		//glRotatef(-30.0f*time, 1.0f, 0.0f, 0.0f);

		//if (mesh!=0 && false/*disabling gigantem logo*/)
		//{
		//	glEnable(GL_POLYGON_SMOOTH);
		//	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		//	mesh->render();
		//	glDisable(GL_POLYGON_SMOOTH);
		//	glEnable(GL_LINE_SMOOTH);
		//	glPolygonMode(GL_FRONT, GL_LINE);
		//	mesh->render();
		//	glPolygonMode(GL_FRONT, GL_FILL);
		//	glDisable(GL_LINE_SMOOTH);
		//}

		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();
		//glOrtho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();

		//glMatrixMode(GL_PROJECTION);
		//glPopMatrix();
		//glMatrixMode(GL_MODELVIEW);
		//glPopMatrix();
	}
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
		return true;
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
		RectangleWindow::handleEvent(pEvent);
		return true;
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
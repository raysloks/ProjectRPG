#include "StandardHUD.h"
#include "Client.h"
#include "GUIObject.h"
#include "World.h"
#include "ClientData.h"

StandardHUD::StandardHUD(World * pWorld, Client * pClient, int px, int py, int width, int height) : RectangleWindow(px, py, width, height), world(pWorld), client(pClient)
{
	last_filled = 1.0f;
}

StandardHUD::~StandardHUD(void)
{
}

#include "Writing.h"

void StandardHUD::render(void)
{
	/*glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	
	if (client->clientData!=0) {
		if (world->GetEntity(client->clientData->getUnit(client->clientData->unit_id))!=0) {
			Entity * ent = world->GetEntity(client->clientData->getUnit(client->clientData->unit_id));
			Unit * unit = dynamic_cast<Unit*>(ent);
			if (unit!=0) {
				Writing::render(std::to_string((long long)unit->health.current) + " / " + std::to_string((long long)unit->health.max));
			}
		}
	}*/

	//glDisable(GL_DEPTH_TEST);
	//glDisable(GL_LIGHTING);
	//glDisable(GL_TEXTURE_2D);

	//float aspect = ((float)w)/((float)h);

	//glMatrixMode(GL_PROJECTION);
	//glPushMatrix();
	//glLoadIdentity();
	//glOrtho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
	//glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	//glLoadIdentity();
	//
	//if (client->clientData!=0) {
	//	if (world->GetEntity(client->clientData->getUnit(client->clientData->unit_id))!=0) {
	//		Entity * ent = 0;//world->GetEntity(client->clientData->getUnit(client->clientData->unit_id));
	//		Unit * unit = dynamic_cast<Unit*>(ent);
	//		if (unit!=0) {

	//			float filled = std::max<float>(unit->health.current, 0.0f) / unit->health.max;

	//			glColor3f(0.2f, 0.3f, 0.2f);
	//			glBegin(GL_QUADS);
	//			glVertex2f(-0.25f, -0.95f);
	//			glVertex2f(0.25f, -0.95f);
	//			glVertex2f(0.25f, -0.90f);
	//			glVertex2f(-0.25f, -0.90f);

	//			glColor3f(1.0f, 0.0f, 0.0f);

	//			glVertex2f(-0.25f, -0.95f);
	//			glVertex2f(-0.25f+last_filled/2.0f, -0.95f);
	//			glVertex2f(-0.25f+last_filled/2.0f, -0.90f);
	//			glVertex2f(-0.25f, -0.90f);

	//			glColor3f(0.3f, 0.6f, 0.3f);

	//			glVertex2f(-0.25f, -0.95f);
	//			glVertex2f(-0.25f+filled/2.0f, -0.95f);
	//			glVertex2f(-0.25f+filled/2.0f, -0.90f);
	//			glVertex2f(-0.25f, -0.90f);
	//			glEnd();

	//			if (last_filled>filled)
	//			{
	//				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//				glBegin(GL_TRIANGLE_FAN);

	//				glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
	//				glVertex2f(0.0f, 0.0f);

	//				glColor4f(1.0f, 0.0f, 0.0f, (last_filled-filled));
	//				glVertex2f(-aspect, -1.0f);
	//				glVertex2f(aspect, -1.0f);
	//				glVertex2f(aspect, 1.0f);
	//				glVertex2f(-aspect, 1.0f);
	//				glVertex2f(-aspect, -1.0f);

	//				glEnd();

	//				last_filled -= 1.0f/60.0f;
	//			}
	//			else
	//			{
	//				last_filled = filled;
	//			}
	//		} else {
	//		}
	//	}
	//}

	//glMatrixMode(GL_PROJECTION);
	//glPopMatrix();
	//glMatrixMode(GL_MODELVIEW);
	//glPopMatrix();
}

bool StandardHUD::handleEvent(IEvent * pEvent)
{
	return false;
}
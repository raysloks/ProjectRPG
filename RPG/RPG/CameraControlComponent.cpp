#include "CameraControlComponent.h"

#include "NewEntity.h"
#include "World.h"
#include "Client.h"

#include "PositionComponent.h"
#include "MobComponent.h"
#include "CameraShakeComponent.h"

#include "BlendUtility.h"

#include "Script.h"
#include "FloatVar.h"

#include "Matrix3.h"
#include "Matrix4.h"

#include "ClientData.h"

const AutoSerialFactory<CameraControlComponent> CameraControlComponent::_factory("CameraControlComponent");

CameraControlComponent::CameraControlComponent(void) : Serializable(_factory.id)
{
}

CameraControlComponent::CameraControlComponent(instream& is, bool full) : Serializable(_factory.id)
{
	cam_rot_basic = Vec2(M_PI_4, M_PI_2 + M_PI_4);
}

CameraControlComponent::~CameraControlComponent(void)
{
}

void CameraControlComponent::connect(NewEntity * pEntity, bool authority)
{
}

void CameraControlComponent::disconnect(void)
{
}

#include "GraphicsComponent.h"

void CameraControlComponent::pre_frame(float dTime)
{
	Client * client = entity->world->client;
	if (client != nullptr)
	{
		if (client->clientData->client_id == client_id || !entity->world->authority)
		{
			if (!p)
				p = entity->getComponent<PositionComponent>();

			//set camera position relative to focus point
			if (p)
			{
				entity->world->cam_rot = cam_rot;

				auto offset = up * 1.25f;

				auto shakes = entity->world->GetNearestComponents<CameraShakeComponent>(p->p);
				for (auto shake : shakes)
				{
					offset += shake.second->getShake() / (shake.first + 1.0f);
				}

				entity->world->cam_pos = p->p + offset;
			}

			auto g = entity->getComponent<GraphicsComponent>();
			if (g)
			{
				g->decs.items.clear();
			}
		}
	}
}

void CameraControlComponent::post_frame(float dTime)
{
	Client * client = entity->world->client;
	if (client != nullptr)
	{
		if (client->clientData->client_id == client_id || !entity->world->authority)
		{
			const Input& input = client->input;

			float sensitivity = 0.0025f;
			float sensitivity_x = 1.0f;
			float sensitivity_y = 1.0f;
			float controller_sensitivity = 2.0f;
			float controller_sensitivity_x = 1.0f;
			float controller_sensitivity_y = 1.0f;
			if (client->mem != 0)
			{
				auto var = std::dynamic_pointer_cast<FloatVar>(client->mem->getVariable("sensitivity"));
				if (var != 0)
					sensitivity = var->f;
				var = std::dynamic_pointer_cast<FloatVar>(client->mem->getVariable("sensitivity_x"));
				if (var != 0)
					sensitivity_x = var->f;
				var = std::dynamic_pointer_cast<FloatVar>(client->mem->getVariable("sensitivity_y"));
				if (var != 0)
					sensitivity_y = var->f;
				var = std::dynamic_pointer_cast<FloatVar>(client->mem->getVariable("controller_sensitivity"));
				if (var != 0)
					controller_sensitivity = var->f;
				var = std::dynamic_pointer_cast<FloatVar>(client->mem->getVariable("controller_sensitivity_x"));
				if (var != 0)
					controller_sensitivity_x = var->f;
				var = std::dynamic_pointer_cast<FloatVar>(client->mem->getVariable("controller_sensitivity_y"));
				if (var != 0)
					controller_sensitivity_y = var->f;
			}

			Vec2 mouse_move = Vec2(input.mouse_dif_x, input.mouse_dif_y);

			mouse_move *= sensitivity;
			mouse_move.x *= sensitivity_x;
			mouse_move.y *= sensitivity_y;

			Vec2 f = client->input.ctrl[0].right_analog.out;

			mouse_move += Vec2(f.x*controller_sensitivity_x, -f.y*controller_sensitivity_y)*controller_sensitivity*dTime;


			cam_rot_basic += mouse_move;
			
			cam_rot_basic.x = std::fmodf(cam_rot_basic.x, M_PI * 2.0f);
			cam_rot_basic.y = std::fminf(M_PI, std::fmaxf(0.0f, cam_rot_basic.y));


			cam_rot = Quaternion();

			cam_rot = Quaternion(cam_rot_basic.y, Vec3(1.0f, 0.0f, 0.0f)) * cam_rot;
			cam_rot = Quaternion(cam_rot_basic.x, Vec3(0.0f, 0.0f, -1.0f)) * cam_rot;

			cam_rot.Normalize();

			forward = Vec3(0.0f, -1.0f, 0.0f) * Quaternion(cam_rot_basic.x, Vec3(0.0f, 0.0f, -1.0f));
			up = Vec3(0.0f, 0.0f, 1.0f);

			front = Vec3(0.0f, 0.0f, 1.0f) * cam_rot;
			top = Vec3(0.0f, 1.0f, 0.0f) * cam_rot;

			right = Vec3(-1.0f, 0.0f, 0.0f) * cam_rot;
		}
	}
}

void CameraControlComponent::writeLog(outstream& os, ClientData& client)
{
}

void CameraControlComponent::readLog(instream& is)
{
}

void CameraControlComponent::writeLog(outstream& os)
{
}

void CameraControlComponent::readLog(instream& is, ClientData& client)
{
}

void CameraControlComponent::interpolate(Component * pComponent, float fWeight)
{
}

void CameraControlComponent::write_to(outstream& os, ClientData& client) const
{
}

void CameraControlComponent::write_to(outstream& os) const
{
}

bool CameraControlComponent::visible(ClientData& client) const
{
	return client.client_id == client_id;
}
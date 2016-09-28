#include "CameraControlComponent.h"

#include "NewEntity.h"
#include "World.h"
#include "Client.h"

#include "PositionComponent.h"

#include "Script.h"
#include "FloatVar.h"

#include "ClientData.h"

const AutoSerialFactory<CameraControlComponent> CameraControlComponent::_factory("CameraControlComponent");

CameraControlComponent::CameraControlComponent(void) : Serializable(_factory.id)
{
	camera.z = 2.0f;
	offset.z = 1.0f;
	local_offset.z = 1.0f;
}

CameraControlComponent::CameraControlComponent(instream& is, bool full) : Serializable(_factory.id)
{
	is >> camera >> offset;
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

void CameraControlComponent::frame(float dTime)
{
	Client * client = entity->world->client;
	if (client!=0)
	{
		if (client->clientData->unit_id == entity->id || !entity->world->authority)
		{
			const Input& input = client->input;

			/*float zoom = input.ctrl[0].left_shoulder.down ? 1.0f : 0.0f;
			zoom -= input.ctrl[0].right_shoulder.down ? 1.0f : 0.0f;

			camera.z += zoom * dTime;

			camera.z -= float(input.mouse_dif_z) / 2400.0f;*/

			if (camera.z < 0.0f)
				camera.z = 0.0f;

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

			if (client->isActive)
				camera -= Vec3(mouse_move.x, mouse_move.y, 0.0f);
			if (camera.x > M_PI * 2) {
				camera.x -= M_PI * 2 * ((int)(camera.x / (M_PI * 2)));
			}
			if (camera.x < 0.0f) {
				camera.x += M_PI * 2 * (1 + (int)(-camera.x / (M_PI * 2)));
			}
			if (camera.y > M_PI / 2.0f) {
				camera.y = M_PI / 2.0f;
			}
			if (camera.y < -M_PI / 2.0f) {
				camera.y = -M_PI / 2.0f;
			}

			if (p == 0) {
				PositionComponent * pc = entity->getComponent<PositionComponent>();
				if (pc != 0)
					p = &pc->p;
			}

			//set camera position relative to focus point
			if (p != 0) {
				lag_position -= *p;
				lag_position -= Vec3(lag_position) * (1.0f - exp(log(0.0001f)*dTime));
				lag_position += *p;

				Vec3 front = Vec3(-sin(camera.x)*cos(camera.y), cos(camera.x)*cos(camera.y), sin(camera.y));
				Vec3 right = Vec3(-cos(camera.x), -sin(camera.x), 0.0f);
				Vec3 up = front.Cross(right);
				Vec3 cam_pos = -front * (exp(camera.z) - 1.0f) + right * local_offset.x + front * local_offset.y + up * local_offset.z;
				entity->world->cam_rot = Quaternion(Vec3(0.0f, 0.0f, camera.x + M_PI)) * Quaternion(Vec3(-camera.y + M_PI_2, 0.0f, 0.0f));
				entity->world->cam_pos = lag_position + offset + cam_pos;
			}
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
	os << camera << offset;
}

void CameraControlComponent::write_to(outstream& os) const
{
}

bool CameraControlComponent::visible(ClientData& client) const
{
	return client.unit_id == entity->id;
}
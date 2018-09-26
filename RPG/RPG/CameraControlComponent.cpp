#include "CameraControlComponent.h"

#include "NewEntity.h"
#include "World.h"
#include "Client.h"

#include "PositionComponent.h"
#include "MobComponent.h"
#include "CameraShakeComponent.h"
#include "GraphicsComponent.h"
#include "ColliderComponent.h"

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

#include "ColliderComponent.h"

void CameraControlComponent::pre_frame(float dTime)
{
	Client * client = entity->world->client;
	if (client != nullptr)
	{
	}
}

void CameraControlComponent::post_frame(float dTime)
{
	Client * client = entity->world->client;
	if (client != nullptr)
	{
		int view_w = client->platform->get_width();
		int view_h = client->platform->get_height();

		float camera_speed = 20.0f;
		if (client->input.mouse_x <= 0)
		{
			p += Vec3(1.0f, 0.0f, 0.0f) * dTime * camera_speed;
		}
		if (client->input.mouse_x >= view_w - 1)
		{
			p += Vec3(-1.0f, 0.0f, 0.0f) * dTime * camera_speed;
		}
		if (client->input.mouse_y <= 0)
		{
			p += Vec3(0.0f, -1.0f, 0.0f) * dTime * camera_speed;
		}
		if (client->input.mouse_y >= view_h - 1)
		{
			p += Vec3(0.0f, 1.0f, 0.0f) * dTime * camera_speed;
		}

		Vec3 offset(0.0f, 0.5f, 1.0f);
		offset *= 20.0f;

		entity->world->cam_pos = p + offset;

		entity->world->cam_rot = Quaternion(M_PI - M_PI / 5.0f, Vec3(1.0f, 0.0f, 0.0f));
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
	return true;
}
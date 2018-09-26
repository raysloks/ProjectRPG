#include "PlayerInputComponent.h"

#include "NewEntity.h"

#include "World.h"

#include "PositionComponent.h"
#include "MobComponent.h"
#include "CameraControlComponent.h"
#include "ColliderComponent.h"

#include "ClientData.h"

#include "Client.h"

#include "Input.h"

#include "Matrix4.h"
#include "Quaternion.h"

const AutoSerialFactory<PlayerInputComponent> PlayerInputComponent::_factory("PlayerInputComponent");

PlayerInputComponent::PlayerInputComponent(void) : Serializable(_factory.id)
{
}

PlayerInputComponent::PlayerInputComponent(instream& is, bool full) : Serializable(_factory.id)
{
}

PlayerInputComponent::~PlayerInputComponent(void)
{
}

void PlayerInputComponent::connect(NewEntity * pEntity, bool authority)
{
}

void PlayerInputComponent::disconnect(void)
{
}

void PlayerInputComponent::post_frame(float dTime)
{
	Client * client = entity->world->client;
	if (client != nullptr)
	{
		if (client->clientData->client_id == client_id || !entity->world->authority)
		{
			int view_w = client->platform->get_width();
			int view_h = client->platform->get_height();

			Vec3 mouse(client->input.mouse_x, client->input.mouse_y, -1.0f);
			mouse -= Vec3(view_w / 2, view_h / 2, 0.0f);
			mouse /= Vec3(view_w / 2, -view_h / 2, 1.0f);

			double aspect = ((double)view_w) / ((double)view_h);

			Matrix4 pers = Matrix4::Perspective(40.0f, aspect, 1.0f, 100.0f);

			Matrix4 rot = entity->world->cam_rot.getConj();
			Matrix4 invrot = entity->world->cam_rot;

			Matrix4 proj = rot * pers;
			Matrix4 proj_inv = proj.Inverse();

			mouse *= proj_inv;

			mouse *= 100.0f;

			GlobalPosition current_target_location;
			EntityID current_target;

			{
				std::vector<std::shared_ptr<Collision>> list;
				ColliderComponent::LineCheck(entity->world->cam_pos, entity->world->cam_pos + mouse, list);
				if (!list.empty())
				{
					std::sort(list.begin(), list.end(), [](const std::shared_ptr<Collision>& a, const std::shared_ptr<Collision>& b) { return a->t < b->t; });

					current_target_location = list.front()->poc;
				}
			}
			
			auto mobs = entity->world->GetNearestComponents<MobComponent>(entity->world->cam_pos);
			for each (auto mob in mobs)
			{
				auto p = mob.second->entity->getComponent<PositionComponent>();
				auto col = Wall::SphereLine(Vec3(), mouse, p->p - entity->world->cam_pos, mob.second->r);
				if (col != nullptr)
				{
					current_target = mob.second->entity->get_id();
				}
			}

			if (client->input.isPressed(Platform::KeyEvent::RMB))
			{
				target_location = current_target_location;
				target = current_target;
			}
		}
	}
}

void PlayerInputComponent::tick(float dTime)
{
	if (!entity->world->authority)
		return;

	auto mob = entity->getComponent<MobComponent>();
	if (mob)
	{
		mob->target_location = target_location;
		mob->target = target;
	}
}

void PlayerInputComponent::writeLog(outstream& os, ClientData& client)
{
}

void PlayerInputComponent::readLog(instream& is)
{
}

void PlayerInputComponent::writeLog(outstream& os)
{
	os << target_location << target;
}

void PlayerInputComponent::readLog(instream& is, ClientData& client)
{
	if (client.client_id == client_id)
	{
		is >> target_location >> target;
		if (target.id != 0xffffffff)
		{
			target.id = client.getRealID(target.id);
			target.uid = entity->world->uid[target.id];
		}
	}
}

void PlayerInputComponent::interpolate(Component * pComponent, float fWeight)
{
}

void PlayerInputComponent::write_to(outstream& os, ClientData& client) const
{
}

void PlayerInputComponent::write_to(outstream& os) const
{
}

bool PlayerInputComponent::visible(ClientData& client) const
{
	return client.client_id == client_id;
}
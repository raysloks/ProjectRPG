#include "PlayerInputComponent.h"

#include "NewEntity.h"

#include "World.h"

#include "PositionComponent.h"
#include "MobComponent.h"
#include "CameraControlComponent.h"

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
	is >> cs;
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
	if (p == nullptr) {
		auto pc = entity->getComponent<PositionComponent>();
		p = &pc->p;
	}
	if (mob == nullptr)
		mob = entity->getComponent<MobComponent>();

	Client * client = entity->world->client;
	if (client != nullptr)
	{
		if (client->clientData->client_id == client_id || !entity->world->authority)
		{
			const Input& input = client->input;

			move = client->input.ctrl[0].left_analog.out;

			if (move.Len() > 1.0f)
				move.Normalize();

			if (client->isActive)
			{
				if (input.isDown(Platform::KeyEvent::W) || input.isDown(Platform::KeyEvent::UP))
					move.y += 1.0f;
				if (input.isDown(Platform::KeyEvent::A) || input.isDown(Platform::KeyEvent::LEFT))
					move.x -= 1.0f;
				if (input.isDown(Platform::KeyEvent::S) || input.isDown(Platform::KeyEvent::DOWN))
					move.y -= 1.0f;
				if (input.isDown(Platform::KeyEvent::D) || input.isDown(Platform::KeyEvent::RIGHT))
					move.x += 1.0f;
			}

			if (move.Len() > 1.0f)
				move.Normalize();

			/*Vec3 local_dir = move;
			move.x = local_dir.x*cos(camera->x) - local_dir.y*sin(camera->x);
			move.y = local_dir.x*sin(camera->x) + local_dir.y*cos(camera->x);*/

			Vec3 fx, fy;

			auto ccc = entity->getComponent<CameraControlComponent>();
			if (ccc != nullptr)
			{
				fx = ccc->right;
				fy = ccc->forward;

				cam_rot = ccc->cam_rot;
			}

			float l = move.Len();
			move = fy * move.y + fx * move.x;
			move.Normalize();
			move *= l;

			cs.input["run"] = input.isDown(Platform::KeyEvent::LSHIFT) || input.ctrl[0].left_trigger.out;
			cs.input["crouch"] = input.isDown(Platform::KeyEvent::LCTRL) || input.ctrl[0].b.down;
			if (input.isPressed(Platform::KeyEvent::SPACE) || input.ctrl[0].a.pressed)
				cs.activate("jump");
			if (input.isPressed(Platform::KeyEvent::LMB) || input.ctrl[0].x.pressed)
				cs.activate("attack");
			if (input.isPressed(Platform::KeyEvent::RMB) || input.ctrl[0].right_trigger.pressed)
				cs.activate("roll");
			if (input.isPressed(Platform::KeyEvent::TAB) || input.ctrl[0].y.pressed)
				cs.activate("switch");
		}
	}
}

void PlayerInputComponent::tick(float dTime)
{
	if (p == nullptr)
	{
		auto pc = entity->getComponent<PositionComponent>();
		p = &pc->p;
	}
	if (mob == nullptr)
		mob = entity->getComponent<MobComponent>();

	if (mob != nullptr)
	{
		mob->move = move;
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
	os << cs << move << cam_rot;
}

void PlayerInputComponent::readLog(instream& is, ClientData& client)
{
	if (client.client_id == client_id)
	{
		ControlState ncs;
		is >> ncs >> move >> cam_rot;
		ncs.update(cs);
	}
}

void PlayerInputComponent::interpolate(Component * pComponent, float fWeight)
{
}

void PlayerInputComponent::write_to(outstream& os, ClientData& client) const
{
	os << cs;
}

void PlayerInputComponent::write_to(outstream& os) const
{
}

bool PlayerInputComponent::visible(ClientData& client) const
{
	return client.client_id == client_id;
}
#include "PlayerInputComponent.h"

#include "NewEntity.h"

#include "World.h"

#include "PositionComponent.h"
#include "MobComponent.h"
#include "CameraControlComponent.h"
#include "AnimationControlComponent.h"
#include "SimpleState.h"

#include "ClientData.h"

#include "Client.h"

#include "Input.h"

#include "Matrix4.h"
#include "Quaternion.h"

AutoSerialFactory<PlayerInputComponent, Component> PlayerInputComponent::_factory("PlayerInputComponent");

PlayerInputComponent::PlayerInputComponent(void) : Component(_factory.id)
{
}

PlayerInputComponent::PlayerInputComponent(instream& is, bool full) : Component(_factory.id)
{
	is >> sc.sync;
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

				facing = ccc->cam_rot_basic;
			}

			float l = move.Len();
			move = fy * move.y + fx * move.x;
			move.Normalize();
			move *= l;

			if (input.isPressed(Platform::KeyEvent::LMB) || input.ctrl[0].x.pressed)
				sc.queue.emplace_back("attack");
			if (input.isPressed(Platform::KeyEvent::RMB) || input.ctrl[0].right_trigger.pressed)
				sc.queue.emplace_back("dash");
			if (input.isPressed(Platform::KeyEvent::SPACE) || input.ctrl[0].a.pressed)
				sc.queue.emplace_back("jump");
			if (input.isPressed(Platform::KeyEvent::Q) || input.ctrl[0].y.pressed)
				sc.queue.emplace_back("heal");
			if (input.isPressed(Platform::KeyEvent::F17))
				sc.queue.emplace_back("fly");
		}
	}
}

void PlayerInputComponent::tick(float dTime)
{
	if (mob == nullptr)
		mob = entity->getComponent<MobComponent>();

	if (mob != nullptr)
	{
		auto acc = entity->getComponent<AnimationControlComponent>();
		if (!acc->has_state("roll"))
			mob->facing = facing;

		if (entity->world->authority)
		{
			mob->move = move;

			float buffer_duration = 0.2f;

			if (sc.consume("attack"))
				mob->input["attack"] = buffer_duration;
			if (sc.consume("dash"))
				mob->input["roll"] = buffer_duration;
			if (sc.consume("jump"))
				mob->input["jump"] = buffer_duration;
			if (sc.consume("fly"))
				mob->input["dash"] = buffer_duration;
			if (sc.consume("heal"))
				mob->input["heal"] = buffer_duration;
		}
	}
}

void PlayerInputComponent::writeLog(outstream& os, ClientData& client)
{
	sc.writeLog(os, client);
}

void PlayerInputComponent::readLog(instream& is)
{
	sc.readLog(is);
}

void PlayerInputComponent::writeLog(outstream& os)
{
	os << move << facing;
	sc.writeLog(os);
}

void PlayerInputComponent::readLog(instream& is, ClientData& client)
{
	if (client.client_id == client_id)
	{
		is >> move >> facing;
		sc.readLog(is, client);
	}
}

void PlayerInputComponent::interpolate(Component * pComponent, float fWeight)
{
	auto other = reinterpret_cast<PlayerInputComponent*>(pComponent);
	sc.update(other->sc.sync);
}

void PlayerInputComponent::write_to(outstream& os, ClientData& client) const
{
	os << sc.sync;
}

void PlayerInputComponent::write_to(outstream& os) const
{
}

bool PlayerInputComponent::visible(ClientData& client) const
{
	return client.client_id == client_id;
}
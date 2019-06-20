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

PlayerInputComponent::PlayerInputComponent(instream& is) : Component(_factory.id)
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

			Vec3 move_input = client->input.ctrl[0].left_analog.out;

			if (client->isActive)
			{
				if (input.isDown(Platform::KeyEvent::W) || input.isDown(Platform::KeyEvent::UP))
					move_input.y += 1.0f;
				if (input.isDown(Platform::KeyEvent::A) || input.isDown(Platform::KeyEvent::LEFT))
					move_input.x -= 1.0f;
				if (input.isDown(Platform::KeyEvent::S) || input.isDown(Platform::KeyEvent::DOWN))
					move_input.y -= 1.0f;
				if (input.isDown(Platform::KeyEvent::D) || input.isDown(Platform::KeyEvent::RIGHT))
					move_input.x += 1.0f;
				if (input.isDown(Platform::KeyEvent::SPACE))
					move_input.z += 1.0f;
				if (input.isDown(Platform::KeyEvent::LCTRL))
					move_input.z -= 1.0f;
			}

			/*Vec3 local_dir = move;
			move.x = local_dir.x*cos(camera->x) - local_dir.y*sin(camera->x);
			move.y = local_dir.x*sin(camera->x) + local_dir.y*cos(camera->x);*/

			auto ccc = entity->getComponent<CameraControlComponent>();

			if (input.isDown(Platform::KeyEvent::RMB))
				facing = ccc->cam_rot_basic;

			move = ccc->forward * move_input.y + ccc->right * move_input.x;
			if (move.Len() > 1.0f)
				move.Normalize();

			move_space = ccc->front * move_input.y + ccc->right * move_input.x + ccc->up * move_input.z;
			if (move_space.Len() > 1.0f)
				move_space.Normalize();

			if (input.isPressed(Platform::KeyEvent::N1) || input.ctrl[0].x.pressed)
				sc.queue.emplace_back("attack");
			if (input.isPressed(Platform::KeyEvent::N2) || input.ctrl[0].right_trigger.pressed)
				sc.queue.emplace_back("dash");
			if (input.isPressed(Platform::KeyEvent::SPACE) || input.ctrl[0].a.pressed)
				sc.queue.emplace_back("jump");
			if (input.isPressed(Platform::KeyEvent::Q) || input.ctrl[0].y.pressed)
				sc.queue.emplace_back("heal");
			if (input.isPressed(Platform::KeyEvent::F1))
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
			mob->move_space = move_space;

			float buffer_duration = 0.2f;

			if (sc.consumeValue("attack"))
				mob->input["attack"] = buffer_duration;
			if (sc.consumeValue("dash"))
				mob->input["roll"] = buffer_duration;
			if (sc.consumeValue("jump"))
				mob->input["jump"] = buffer_duration;
			if (sc.consumeValue("fly"))
				mob->input["dash"] = buffer_duration;
			if (sc.consumeValue("heal"))
				mob->input["heal"] = buffer_duration;
		}
	}
}

void PlayerInputComponent::writeLog(outstream& os, const std::shared_ptr<ClientData>& client)
{
	sc.writeFromDestination(os);
}

void PlayerInputComponent::readLog(instream& is)
{
	sc.readFromDestination(is);
}

void PlayerInputComponent::writeLog(outstream& os)
{
	os << move << move_space << facing;
	sc.writeFromSource(os);
}

void PlayerInputComponent::readLog(instream& is, const std::shared_ptr<ClientData>& client)
{
	if (client->client_id == client_id)
	{
		is >> move >> move_space >> facing;
		sc.readFromSource(is);
	}
}

void PlayerInputComponent::interpolate(Component * pComponent, float fWeight)
{
	auto other = reinterpret_cast<PlayerInputComponent*>(pComponent);
	sc.update(other->sc.sync);
}

void PlayerInputComponent::write_to(outstream& os, const std::shared_ptr<ClientData>& client) const
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
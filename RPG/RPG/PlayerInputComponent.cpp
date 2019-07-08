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

#include "Ability.h"
#include "AbilityContext.h"

AutoSerialFactory<PlayerInputComponent, Component> PlayerInputComponent::_factory("PlayerInputComponent");

PlayerInputComponent::PlayerInputComponent(void) : Component(_factory.id)
{
}

PlayerInputComponent::PlayerInputComponent(instream& is) : Component(_factory.id)
{
	is >> sc.sync >> scn.sync;
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
		if (client->clientData == clientData.lock() || !entity->world->authority)
		{
			const Input& input = client->input;

			Vec3 move_input = client->input.ctrl[0].left_analog.out;

			if (client->isActive)
			{
				if (input.isDown(Platform::KeyEvent::W) || input.isDown(Platform::KeyEvent::UP) || input.isDown(Platform::KeyEvent::LMB) && input.isDown(Platform::KeyEvent::RMB))
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

			Vec3 forward = Vec3(0.0f, -1.0f, 0.0f) * Quaternion(facing.x, Vec3(0.0f, 0.0f, -1.0f));
			Vec3 up = Vec3(0.0f, 0.0f, 1.0f);
			Vec3 right = forward.Cross(up);

			move = forward * move_input.y + right * move_input.x;
			if (move.Len() > 1.0f)
				move.Normalize();

			move_space = ccc->front * move_input.y + ccc->right * move_input.x + ccc->up * move_input.z;
			if (move_space.Len() > 1.0f)
				move_space.Normalize();

			/*if (input.isPressed(Platform::KeyEvent::N1) || input.ctrl[0].x.pressed)
				sc.queue.emplace_back("attack");
			if (input.isPressed(Platform::KeyEvent::N2) || input.ctrl[0].right_trigger.pressed)
				sc.queue.emplace_back("dash");
			if (input.isPressed(Platform::KeyEvent::Q) || input.ctrl[0].y.pressed)
				sc.queue.emplace_back("heal");*/

			if (input.isPressed(Platform::KeyEvent::N1))
				scn.queue.emplace_back(AbilityUseCommand{ 0, target });
			if (input.isPressed(Platform::KeyEvent::N2))
				scn.queue.emplace_back(AbilityUseCommand{ 1, target });
			if (input.isPressed(Platform::KeyEvent::N3))
				scn.queue.emplace_back(AbilityUseCommand{ 2, target });
			if (input.isPressed(Platform::KeyEvent::N4))
				scn.queue.emplace_back(AbilityUseCommand{ 3, target });
			if (input.isPressed(Platform::KeyEvent::Q))
				scn.queue.emplace_back(AbilityUseCommand{ 4, target });
			if (input.isPressed(Platform::KeyEvent::E))
				scn.queue.emplace_back(AbilityUseCommand{ 5, target });
			if (input.isPressed(Platform::KeyEvent::R))
				scn.queue.emplace_back(AbilityUseCommand{ 6, target });
			if (input.isPressed(Platform::KeyEvent::F))
				scn.queue.emplace_back(AbilityUseCommand{ 7, target });

			if (input.isPressed(Platform::KeyEvent::SPACE) || input.ctrl[0].a.pressed)
				sc.queue.emplace_back("jump");
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

			auto client = clientData.lock();

			for (auto command : scn.queue)
			{
				AbilityContext ac;
				ac.source = mob;

				EntityID entity_id = command.target;
				entity_id = client->getRealID(entity_id);
				if (entity_id.id < entity->world->uid.size())
					entity_id.uid = entity->world->uid[entity_id.id];

				auto ent = entity->world->GetEntity(entity_id);
				if (ent)
					ac.target = ent->getComponent<MobComponent>();
				if (ac.target != nullptr)
					Ability::get(*mob->abilities.items[command.ability_index])->activate(ac);
			}
			scn.queue.clear();

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
	scn.writeFromDestination(os);
}

void PlayerInputComponent::readLog(instream& is)
{
	sc.readFromDestination(is);
	scn.readFromDestination(is);
}

void PlayerInputComponent::writeLog(outstream& os)
{
	os << move << move_space << facing;
	sc.writeFromSource(os);
	scn.writeFromSource(os);
}

void PlayerInputComponent::readLog(instream& is, const std::shared_ptr<ClientData>& client)
{
	if (client == clientData.lock())
	{
		is >> move >> move_space >> facing;
		sc.readFromSource(is);
		scn.readFromSource(is);
	}
}

void PlayerInputComponent::interpolate(Component * pComponent, float fWeight)
{
	auto other = reinterpret_cast<PlayerInputComponent*>(pComponent);
	sc.update(other->sc.sync);
	scn.update(other->scn.sync);
}

void PlayerInputComponent::write_to(outstream& os, const std::shared_ptr<ClientData>& client) const
{
	os << sc.sync << scn.sync;
}

void PlayerInputComponent::write_to(outstream& os) const
{
}

bool PlayerInputComponent::visible(const std::shared_ptr<ClientData>& client) const
{
	return client == clientData.lock();
}
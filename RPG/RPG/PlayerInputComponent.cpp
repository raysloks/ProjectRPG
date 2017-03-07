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

void PlayerInputComponent::frame(float dTime)
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
		if (client->clientData->unit_id == entity->id || !entity->world->authority)
		{
			const Input& input = client->input;

			Vec3 move = client->input.ctrl[0].left_analog.out;

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

			/*Vec3 local_dir = move;
			move.x = local_dir.x*cos(camera->x) - local_dir.y*sin(camera->x);
			move.y = local_dir.x*sin(camera->x) + local_dir.y*cos(camera->x);*/

			Vec3 fx, fy;

			auto ccc = entity->getComponent<CameraControlComponent>();
			if (ccc != nullptr)
			{
				fx = ccc->right;
				fy = ccc->front;

				mob->cam_facing = ccc->front;
			}

			float l = move.Len();
			move = fy * move.y + fx * move.x;
			move.Normalize();
			move *= l;

			cs.input["x"] = move.x;
			cs.input["y"] = move.y;
			cs.input["z"] = move.z;

			cs.input["run"] = input.isDown(Platform::KeyEvent::LSHIFT) || input.ctrl[0].b.down;
			if (input.isPressed(Platform::KeyEvent::SPACE) || input.ctrl[0].a.pressed)
				cs.activate("jump");
			if (input.isPressed(Platform::KeyEvent::LMB) || input.ctrl[0].right_trigger.pressed)
				cs.activate("attack");
			if (input.isPressed(Platform::KeyEvent::Q) || input.ctrl[0].right_thumb.pressed)
				cs.activate("strafe");

			if (input.isPressed(Platform::KeyEvent::H) || input.ctrl[0].up.pressed)
				cs.activate("warp");
		}
	}
}

void PlayerInputComponent::tick(float dTime)
{
	if (p == nullptr) {
		auto pc = entity->getComponent<PositionComponent>();
		p = &pc->p;
	}
	if (mob == nullptr)
		mob = entity->getComponent<MobComponent>();
	if (camera == nullptr) {
		auto ccc = entity->getComponent<CameraControlComponent>();
		if (ccc != nullptr)
			camera = &ccc->camera;
	}

	if (entity->world->authority)
	{
		if (mob != nullptr)
		{
			float buffer_duration = 0.4f;

			mob->move = Vec3(cs.input["x"], cs.input["y"], cs.input["z"]);

			mob->run = cs.input["run"];
			if (cs.active.find("jump") != cs.active.end())
				mob->input["jump"] = buffer_duration;

			if (cs.active.find("attack") != cs.active.end())
				mob->input["attack"] = buffer_duration;
			if (cs.active.find("strafe") != cs.active.end())
				mob->strafe = !mob->strafe;

			mob->cam_facing = Vec3(-sin(cs.input["facing"]), cos(cs.input["facing"]), 0.0f);

			if (cs.active.find("warp") != cs.active.end())
				mob->input["warp"] = buffer_duration;

			cs.active.erase("attack");
			cs.active.erase("strafe");
			cs.active.erase("jump");

			cs.active.erase("warp");
		}
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
	os << cs;
}

void PlayerInputComponent::readLog(instream& is, ClientData& client)
{
	ControlState ncs;
	is >> ncs;
	ncs.update(cs);
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
	return client.unit_id == entity->id;
}
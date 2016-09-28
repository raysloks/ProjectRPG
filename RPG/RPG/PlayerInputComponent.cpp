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
	if (p==0) {
		auto pc = entity->getComponent<PositionComponent>();
		p = &pc->p;
	}
	if (mob==0)
		mob = entity->getComponent<MobComponent>();
	if (camera==0) {
		auto ccc = entity->getComponent<CameraControlComponent>();
		if (ccc != 0)
			camera = &ccc->camera;
	}

	Client * client = entity->world->client;
	if (client!=0)
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

			Matrix4 mat = Matrix4(Quaternion(camera->x, Vec3(0.0f, 0.0f, 1.0f)) * Quaternion(camera->y, Vec3(1.0f, 0.0f, 0.0f)));

			Vec3 xm = Vec3(1.0f, 0.0f, 0.0f) * mat;

			Vec3 fy = mob->up.Cross(xm);
			Vec3 fx = fy.Cross(mob->up);

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
			if (input.isPressed(Platform::KeyEvent::SPACE) || input.ctrl[0].b.pressed || input.isReleased(Platform::KeyEvent::SPACE) || input.ctrl[0].b.released)
				cs.activate("shift");
			if (input.isPressed(Platform::KeyEvent::H) || input.ctrl[0].up.pressed)
				cs.activate("warp");
			if (input.isPressed(Platform::KeyEvent::LMB) || input.ctrl[0].right_trigger.pressed)
				cs.activate("attack");
			if (input.isPressed(Platform::KeyEvent::Q) || input.ctrl[0].right_thumb.pressed)
				cs.activate("strafe");

			cs.input["facing"] = camera->x;
			mob->cam_facing = Vec3(-sin(camera->x), cos(camera->x), 0.0f);
		}
	}

	if (entity->world->authority)
	{
		if (mob!=0)
		{
			float buffer_duration = 0.4f;
			float jump_delay = 0.3f;
			float dodge_delay = 0.2f;
			float dodge_duration = 0.15f;

			mob->move = Vec3(cs.input["x"], cs.input["y"], cs.input["z"]);

			/*mob->run = cs.input["run"];
			if (cs.active.find("jump") != cs.active.end())
				mob->input["jump"] = buffer_duration;*/

			if (cs.active.find("shift") != cs.active.end())
			{
				bool prev = run;
				int shift = cs.active.find("shift")->second;
				run ^= shift % 2;

				int toggled_on = shift / 2 + !prev ? 1 : 0;
				int toggled_off = shift / 2 + prev ? 1 : 0;

				if (toggled_on)
					mob->input["dodge_delay"] = 1.0f;
				if (toggled_off && mob->run)
					mob->input["jump_delay"] = 1.0f;

				cs.active.erase("shift");
			}

			if (mob->input.find("jump_delay") != mob->input.end())
			{
				if (mob->input["jump_delay"] < 1.0f - jump_delay)
				{
					if (run)
						mob->input["jump"] = buffer_duration;
					mob->input.erase("jump_delay");
				}
			}

			if (mob->input.find("dodge_delay") != mob->input.end())
			{
				if (mob->input["dodge_delay"] < 1.0f - dodge_delay && mob->input.find("jump_delay") == mob->input.end())
				{
					if (!run)
						mob->input["dodge"] = buffer_duration;
					mob->input.erase("dodge_delay");
				}
			}

			mob->run = run && mob->input.find("dodge_delay") == mob->input.end() || mob->input.find("jump_delay") != mob->input.end();

			if (cs.active.find("warp") != cs.active.end())
				mob->input["warp"] = buffer_duration;
			mob->cam_facing = Vec3(-sin(cs.input["facing"]), cos(cs.input["facing"]), 0.0f);
			if (cs.active.find("attack") != cs.active.end())
				mob->input["attack"] = buffer_duration;
			if (cs.active.find("strafe") != cs.active.end())
				mob->strafe = !mob->strafe;

			cs.active.erase("warp");
			cs.active.erase("attack");
			cs.active.erase("strafe");
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
}

void PlayerInputComponent::write_to(outstream& os) const
{
}

bool PlayerInputComponent::visible(ClientData& client) const
{
	return client.unit_id == entity->id;
}
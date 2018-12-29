#include "AudioComponent.h"

#include "NewEntity.h"
#include "World.h"
#include "AudioSource.h"
#include "PositionComponent.h"

#include "ClientData.h"

AutoSerialFactory<AudioComponent, Component> AudioComponent::_factory("AudioComponent");

AudioComponent::AudioComponent(const std::string& sound, float pow) : Component(_factory.id), _sound(sound), gain(pow), src(nullptr), offset(0.0f)
{
}

AudioComponent::AudioComponent(instream& is, bool full) : Component(_factory.id), src(nullptr)
{
	is >> _sound >> offset >> gain >> pos_id;
}

AudioComponent::~AudioComponent(void)
{
	if (src != nullptr)
		delete src;
}

void AudioComponent::connect(NewEntity * pEntity, bool authority)
{
	Resource::load(_sound);
	/*if (pEntity->world->client != nullptr)
		Resource::load(_sound);
	else
		Resource::load(_sound, { "no_buffer" });*/
}

void AudioComponent::disconnect(void)
{
}

void AudioComponent::pre_frame(float dTime)
{
	PositionComponent * p = nullptr;
	auto ent = entity->world->GetEntity(pos_id);
	if (ent)
		p = ent->getComponent<PositionComponent>();
	else
		p = entity->getComponent<PositionComponent>();

	if (src == nullptr)
	{
		auto sound = Resource::get<Sound>(_sound);
		if (sound != nullptr)
		{
			src = new AudioSource(sound);
			alSourcef(src->source, AL_GAIN, gain);
			alSourcef(src->source, AL_SAMPLE_OFFSET, offset * src->sound->sample_rate);
			src->Play();
		}
	}
	if (src != nullptr)
	{
		if (p != nullptr)
			src->SetPosition(p->p - entity->world->cam_pos);
	}
}

void AudioComponent::tick(float dTime)
{
	if (!entity->world->authority)
		return;
	if (src == nullptr)
	{
		auto sound = Resource::get<Sound>(_sound);
		if (sound != nullptr)
		{
			src = new AudioSource(0, sound);
		}
	}
	if (src != nullptr)
		if (offset > src->sound->duration)
			entity->world->SetEntity(entity->id, nullptr);
	offset += dTime;
}

void AudioComponent::writeLog(outstream& os, ClientData& client)
{
}

void AudioComponent::readLog(instream& is)
{
}

void AudioComponent::writeLog(outstream& os)
{
}

void AudioComponent::readLog(instream& is, ClientData& client)
{
}

void AudioComponent::interpolate(Component * pComponent, float fWeight)
{
}

void AudioComponent::write_to(outstream& os, ClientData& client) const
{
	os << _sound << offset << gain;
	uint32_t client_side_id = client.getUnit(pos_id.id);
	if (client_side_id != 0xffffffff)
		os << EntityID(client_side_id, client.unit_uid[client_side_id]);
	else
		os << EntityID();
}

void AudioComponent::write_to(outstream& os) const
{
	os << _sound << offset << gain << pos_id;
}
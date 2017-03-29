#include "AudioComponent.h"

#include "NewEntity.h"
#include "World.h"
#include "AudioSource.h"
#include "PositionComponent.h"

const AutoSerialFactory<AudioComponent> AudioComponent::_factory("AudioComponent");

AudioComponent::AudioComponent(const std::string& sound, float pow) : Serializable(_factory.id), _sound(sound), gain(pow), src(nullptr), p(nullptr)
{
}

AudioComponent::AudioComponent(instream& is, bool full) : Serializable(_factory.id), src(nullptr), p(nullptr)
{
	is >> _sound >> offset >> gain;
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
	if (p == nullptr)
	{
		auto pos = entity->getComponent<PositionComponent>();
		if (pos != nullptr)
			p = pos;
	}

	if (src == nullptr)
	{
		auto sound = Resource::get<Sound>(_sound);
		if (sound != nullptr)
		{
			src = new AudioSource(sound);
			alSourcef(src->source, AL_GAIN, gain);
			alSourcef(src->source, AL_SAMPLE_OFFSET, offset*src->sound->sample_rate);
			src->Play();
		}
	}
	if (src != nullptr)
	{
		if (p != nullptr)
			src->SetPosition(p->p - entity->world->cam_pos);
	}

	offset += dTime;
}

void AudioComponent::tick(float dTime)
{
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
}

void AudioComponent::write_to(outstream& os) const
{
	os << _sound << offset << gain;
}
#include "AudioParticle.h"

#include "TestServer.h"
#include "TestClient.h"
#include "World.h"
extern TestServer * server;
extern TestClient * client;

#include "AudioSource.h"

const AutoSerialFactory<AudioParticle> AudioParticle::_factory_AudioParticle("AudioParticle");

AudioParticle::AudioParticle(const GlobalPosition& pos, const Vec3& vel, const std::string& sound, float pow)
	: Serializable(AudioParticle::_factory_AudioParticle.id), PNEntity(pos), v(vel), _sound(sound), time(0.0f), gain(pow)
{
	std::set<std::string> options;
	if (client==0)
		options.insert("no_buffer");
	Resource::load(_sound, options);
}

AudioParticle::AudioParticle(instream& is, bool full) : Serializable(AudioParticle::_factory_AudioParticle.id), PNEntity(is, full)
{
	is >> v >> _sound >> time >> gain;
	Resource::load(_sound);
}

AudioParticle::~AudioParticle(void)
{
}

void AudioParticle::tick(float dTime)
{
	if (world->authority)
	{
		auto sound = Resource::get<Sound>(_sound);
		if (sound!=0)
		{
			if (time>sound->duration)
			{
				//world->SetEntity(id, 0);
			}
		}
		time += dTime;
		p += v*dTime;
	}
}

void AudioParticle::render(const GlobalPosition& origin) //TODO mabe move this to tick to avoid multiple repeats per frame(alternatively use rendering options)
{
	if (src==0)
	{
		auto sound = Resource::get<Sound>(_sound);
		if (sound!=0)
		{
			src.reset(new AudioSource(sound));
			alSourcef(src->source, AL_GAIN, gain);
			if (time<sound->duration) {
				alSourcef(src->source, AL_SAMPLE_OFFSET, time*sound->sample_rate);
				src->Play();
			}
		}
	}

	if (src!=0)
	{
		Vec3 dif = p-origin;
		alSource3f(src->source, AL_POSITION, dif.x, dif.y, dif.z);
	}
}

void AudioParticle::write_to(outstream& os, bool full) const
{
	PNEntity::write_to(os, full);
	os << v << _sound << time << gain;
}
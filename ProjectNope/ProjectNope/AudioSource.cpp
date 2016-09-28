#include "AudioSource.h"

AudioSource::AudioSource(const std::shared_ptr<Sound>& snd) : source(0), sound(snd)
{
	if (sound->getBuffer()!=0)
	{
		alGenSources(1, &source);
		alGetError();
		alSourcei(source, AL_BUFFER, sound->getBuffer());
	}
}

AudioSource::~AudioSource(void)
{
	if (source!=0)
		alDeleteSources(1, &source);
}

void AudioSource::Play(void)
{
	if (source!=0)
		alSourcePlay(source);
}

void AudioSource::SetPosition(const Vec3 & position)
{
	if (source != 0)
		alSource3f(source, AL_POSITION, position.x, position.y, position.z);
}
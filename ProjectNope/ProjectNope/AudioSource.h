#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H

#include "Sound.h"
#include "Vec3.h"

class AudioSource
{
public:
	AudioSource(const std::shared_ptr<Sound>& sound);
	~AudioSource(void);

	void Play(void);
	void SetPosition(const Vec3& position);

	ALuint source;
	std::shared_ptr<Sound> sound;
};

#endif
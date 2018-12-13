#ifndef SOUND_H
#define SOUND_H

#include "Resource.h"

#include <al.h>

#include "streams.h"
#include "Vec3.h"

class Sound
	: public Resource
{
public:
	Sound(void);
	Sound(instream& is, const std::vector<std::string>& options = std::vector<std::string>());
	~Sound(void);

	static void init(void);
	static void release(void);
	static bool isReady(void);

	static void setListenerOrientation(const Vec3& front, const Vec3& up);
	static void setListenerPosition(const Vec3& position);

	ALuint getBuffer(void);

	ALuint buffer;
	void * data;
	unsigned int data_size, sample_rate;
	ALenum format;
	float duration;
};

#endif
#include "Resource.h"
#include "streams.h"
#include <al.h>

#ifndef SOUND_H
#define SOUND_H

class Sound
	: public Resource
{
public:
	Sound(void);
	Sound(instream& is, const std::set<std::string>& options = std::set<std::string>());
	~Sound(void);

	static void init(void);
	static void release(void);
	static bool isReady(void);

	ALuint getBuffer(void);

	ALuint buffer;
	void * data;
	unsigned int data_size, sample_rate;
	ALenum format;
	float duration;
};

#endif
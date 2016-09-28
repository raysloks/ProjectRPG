#include "PNEntity.h"

#ifndef AUDIO_PARTICLE_H
#define AUDIO_PARTICLE_H

class AudioSource;

class AudioParticle
	: public PNEntity
{
public:
	AudioParticle(const GlobalPosition& pos, const Vec3& vel, const std::string& sound, float gain = 1.0f);
	AudioParticle(instream& is, bool full);
	~AudioParticle(void);

	static const AutoSerialFactory<AudioParticle> _factory_AudioParticle;

	void tick(float dTime);
	void render(const GlobalPosition& origin);

	void write_to(outstream& os, bool full) const;

	Vec3 v;
	std::string _sound;
	std::shared_ptr<AudioSource> src;
	float time, gain;
};

#endif
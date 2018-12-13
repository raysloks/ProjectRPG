#include "Sound.h"
#include <alc.h>

ALCdevice * dev = 0;
ALCcontext  * con = 0;

float clamp(float x, float lowerlimit, float upperlimit) {
	if (x < lowerlimit)
		x = lowerlimit;
	if (x > upperlimit)
		x = upperlimit;
	return x;
}

float smoothstep(float edge0, float edge1, float x) {
	// Scale, bias and saturate x to 0..1 range
	x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	// Evaluate polynomial
	return x * x * (3 - 2 * x);
}

Sound::Sound(void)
{
	buffer = 0;

	float freq = 440.0f;
	float freq2 = 880.0f;
	float freq3 = 1000.0f;
	duration = 2.0f;
	sample_rate = 22050;
	size_t buf_size = duration * sample_rate;
	data_size = buf_size * 2;

	data = new char[data_size];
	short * samples = reinterpret_cast<short*>(data);
	for (size_t i = 0; i < buf_size; ++i)
	{
		float t = float(i) / sample_rate;
		float delta_t = t - 0.1f;
		if (delta_t > 0.0f)
			delta_t /= 1.9f;
		if (delta_t < 0.0f)
			delta_t /= 0.1f;
		float modifier = 1.0f - abs(delta_t);
		modifier = smoothstep(0.0f, 1.0f, modifier);
		modifier = fmaxf(0.0f, fminf(1.0f, modifier));
		samples[i] = 32760.0f * 0.25f * sin((2.0f * float(M_PI) * freq) / sample_rate * i) * modifier;
		samples[i] += 32760.0f * 0.15f * sin((2.0f * float(M_PI) * freq2) / sample_rate * i) * modifier;
		samples[i] += 32760.0f * 0.05f * sin((2.0f * float(M_PI) * freq3) / sample_rate * i) * modifier;
	}

	format = AL_FORMAT_MONO16;
}

Sound::Sound(instream& is, const std::vector<std::string>& options)
{
	buffer = 0;

	char id[5];
	id[4] = 0;
	unsigned int size, fmt_size, avg_bytes_per_sec;
	short fmt_tag, channels, block_align, bits_per_sample;

	is.read(id, 4);
	if (!strcmp(id, "RIFF"))
	{
		is >> size;
		is.read(id, 4);
		if (!strcmp(id, "WAVE"))
		{
			is.read(id, 4);
			is >> fmt_size >> fmt_tag >> channels >> sample_rate >> avg_bytes_per_sec >> block_align >> bits_per_sample;

			is.read(id, 4);
			if (!strcmp(id, "data"))
			{
				is >> data_size;
				if (std::find(options.begin(), options.end(), "no_buffer") == options.end())
				{
					data = new char[data_size];
					is.read((char*)data, data_size);
				}
			}

			if (bits_per_sample==8)
			{
				if (channels==1)
					format = AL_FORMAT_MONO8;
				else
					format = AL_FORMAT_STEREO8;
			}
			else
			{
				if (channels==1)
					format = AL_FORMAT_MONO16;
				else
					format = AL_FORMAT_STEREO16;
			}

			duration = data_size * 8.0f / (sample_rate*channels*bits_per_sample);
		}
	}
}

void Sound::init(void)
{
	if (dev == 0)
	{
		dev = alcOpenDevice(nullptr);
	}
	if (dev != 0)
	{
		if (con == 0)
		{
			con = alcCreateContext(dev, nullptr);
			alcMakeContextCurrent(con);
		}
	}
}

void Sound::release(void)
{
	if (con != 0)
	{
		alcMakeContextCurrent(NULL);
		alcDestroyContext(con);
	}
	if (dev != 0)
	{
		alcCloseDevice(dev);
	}
}

bool Sound::isReady(void)
{
	return dev!=0 && con!=0;
}

void Sound::setListenerOrientation(const Vec3& front, const Vec3& up)
{
	if (isReady())
	{
		ALfloat val[6];
		val[0] = front.x;
		val[1] = front.y;
		val[2] = front.z;
		val[3] = up.x;
		val[4] = up.y;
		val[5] = up.z;
		alListenerfv(AL_ORIENTATION, val);
	}
}

void Sound::setListenerPosition(const Vec3& position)
{
	if (isReady())
	{
		ALfloat pos[3];
		pos[0] = position.x;
		pos[1] = position.y;
		pos[2] = position.z;
		alListenerfv(AL_POSITION, pos);
	}
}

ALuint Sound::getBuffer(void)
{
	if (buffer==0)
	{
		if (data!=0)
		{
			alGenBuffers(1, &buffer);
			alBufferData(buffer, format, data, data_size, sample_rate);
		}
	}

	return buffer;
}

Sound::~Sound(void)
{
	if (buffer!=0)
		alDeleteBuffers(1, &buffer);
	if (data!=0)
		delete []data;
}
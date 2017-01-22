#include "Sound.h"
#include <alc.h>

ALCdevice * dev = 0;
ALCcontext  * con = 0;

Sound::Sound(void)
{
	buffer = 0;
	alGenBuffers(1, &buffer);

	float freq = 440.f;
    int seconds = 4;
    unsigned sample_rate = 22050;
    size_t buf_size = seconds * sample_rate;

    short *samples;
    samples = new short[buf_size];
    for(int i=0; i<buf_size; ++i) {
        samples[i] = 32760 * 0.5 * sin( (2.f*float(M_PI)*freq)/sample_rate * i );
    }

    alBufferData(buffer, AL_FORMAT_MONO16, samples, buf_size, sample_rate);

	delete []samples;
}

Sound::Sound(instream& is, const std::set<std::string>& options)
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
				if (options.find("no_buffer")==options.end())
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
#include "ParticleSystem.h"
#include "GUIObject.h"
#include "Texture.h"

const AutoSerialFactory<ParticleSystem> ParticleSystem::_factory_ParticleSystem("ParticleSystem");

ParticleSystem::ParticleSystem(const GlobalPosition& pos) : PNEntity(pos), Serializable(ParticleSystem::_factory_ParticleSystem.id)
{
	seed = rand();

	dir = 10.0f*rand()/(float)RAND_MAX;

	t = 0.0f;
}

ParticleSystem::ParticleSystem(instream& is, bool full) : PNEntity(is, full), Serializable(ParticleSystem::_factory_ParticleSystem.id)
{
	is >> seed >> dir >> t;
}

ParticleSystem::~ParticleSystem(void)
{
}

void ParticleSystem::writeLog(outstream& os)
{
	PNEntity::writeLog(os);
}

void ParticleSystem::readLog(instream& is)
{
	PNEntity::readLog(is);
}

void ParticleSystem::remove(size_t index)
{
	sys[index].e = false;
	alloc.push(index);
}

size_t ParticleSystem::add(void)
{
	if (alloc.empty()) {
		sys.push_back(Particle());
		return sys.size()-1;
	} else {
		size_t val = alloc.top();
		sys[val] = Particle();
		alloc.pop();
		return val;
	}
}

void ParticleSystem::tick(float dTime)
{
	//dTime *= 0.1f;
	
	t += dTime;
	if (t>0.0f)
	{
		t -= 1.0f/60.0f;
		for (int i=0;i<1;++i)
		{
			srand(seed);
			seed += rand();
			seed += 23;
			seed += rand();
			seed += 13;
			srand(seed);

			dir = dir % 10;
			float d = ((float)++dir)*M_PI/5.0f+M_PI*0.05f*rand()/(float)RAND_MAX;
			float v = 2.0f*rand()/(float)RAND_MAX+3.0f;
			float zv = 2.0f*rand()/(float)RAND_MAX+3.0f;
			if (alloc.size()!=sys.size()) {
				size_t index = add();
				sys[index].v = Vec3(cos(d)*v, sin(d)*v, zv);
				index = add();
				sys[index].v = Vec3(cos(d)*v, sin(d)*v, zv);
			} else {
				size_t index = add();
				sys[index].v = Vec3(cos(d)*v, sin(d)*v, zv);
			}
		}
	}

	for (auto i=sys.begin();i!=sys.end();++i)
	{
		i->p+=i->v*dTime;
		i->v.z-=9.8f*dTime;
	}
	for (auto i=sys.begin();i!=sys.end();++i)
	{
		i->t+=dTime;
		if (i->t>1.5f && i->e)
			remove(std::distance(sys.begin(), i));
	}
}

#include "Profiler.h"

void ParticleSystem::render(const GlobalPosition& origin)
{
	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	glPushMatrix();

	Vec3 pos = p-origin;
	glTranslatef(pos.x, pos.y, pos.z);

	std::shared_ptr<Texture> tex = Resource::get<Texture>("data/assets/test.tga");
	if (tex!=0) {
		tex->sRGB = true;
		tex->useGeneralAnisotropicFiltering = false;
		tex->fAnisotropicFiltering = 1.0f;
		glBindTexture(GL_TEXTURE_2D, tex->getGLTexID());
	}

	glDisable(GL_BLEND);

	glAlphaFunc(GL_GREATER, 0.5f);

	glBegin(GL_QUADS);
	for (auto i=sys.begin();i!=sys.end();++i)
	{
		if (i+1==sys.end())
			break;

		Vec3 d = i->v;
		d.Normalize();
		d *= min(1.0f, i->t);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(i->p.x, i->p.y, i->p.z);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(i->p.x-d.x, i->p.y-d.y, i->p.z-d.z);

		++i;

		d = i->v;
		d.Normalize();
		d *= min(1.0f, i->t);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(i->p.x-d.x, i->p.y-d.y, i->p.z-d.z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(i->p.x, i->p.y, i->p.z);
	}
	for (auto i=sys.begin();i!=sys.end();++i)
	{
		if (i+1==sys.end())
			break;

		Vec3 d = i->v;
		d.Normalize();
		d *= min(1.0f, i->t);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(i->p.x-d.x, i->p.y-d.y, i->p.z-d.z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(i->p.x, i->p.y, i->p.z);

		++i;

		d = i->v;
		d.Normalize();
		d *= min(1.0f, i->t);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(i->p.x, i->p.y, i->p.z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(i->p.x-d.x, i->p.y-d.y, i->p.z-d.z);
	}
	glEnd();

	glAlphaFunc(GL_GREATER, 0.0f);

	glEnable(GL_BLEND);

	glPopMatrix();

	QueryPerformanceCounter(&end);
	double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
	Profiler::add("particle_render", durationInSeconds);
}

void ParticleSystem::interpolate(Entity * pEntity, float fWeight)
{
	PNEntity::interpolate(pEntity, fWeight);
}

void ParticleSystem::write_to(outstream& os, bool full) const
{
	PNEntity::write_to(os, full);
	os << seed << dir << t;
}
#include "Polynome.h"
#include "GUIObject.h"
#include "Vec3.h"

Polynome::Polynome(void)
{
}

Polynome::~Polynome(void)
{
}

Vec3 Polynome::get(float t)
{
	Vec3 ret;
	for (int i=0;i<k.size();++i)
	{
		Vec3 add = k[i];
		for (int j=0;j<i;++j)
			add *= t;
		ret += add;
	}
	return ret;
}

void Polynome::render(void)
{
	glBegin(GL_LINES);
	
	Vec3 p = get(start);
	glVertex3f(p.x, p.y, p.z);
	float end = 5.0f;
	for (float t=start;t<end;t+=0.1f)
	{
		p = get(t);
		glVertex3f(p.x, p.y, p.z);
		glVertex3f(p.x, p.y, p.z);
	}
	p = get(end);
	glVertex3f(p.x, p.y, p.z);

	glEnd();
}
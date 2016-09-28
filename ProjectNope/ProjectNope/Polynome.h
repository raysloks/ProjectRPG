#ifndef POLYNOME_H
#define POLYNOME_H

#include "Vec3.h"
#include <vector>

class Polynome
{
public:
	Polynome(void);
	~Polynome(void);

	Vec3 get(float t);
	void render(void);

	std::vector<Vec3> k;
	float start, end;
};

#endif
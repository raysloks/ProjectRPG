#include "Collision.h"

Collision::Collision(void) : comp(nullptr)
{
}

bool Collision::operator==(const Collision& rhs) const
{
	return poo == rhs.poo &&
		poc == rhs.poc &&
		n == rhs.n &&
		disp == rhs.disp &&
		v == rhs.v &&
		dist == rhs.dist &&
		t == rhs.t &&
		comp == rhs.comp &&
		wall == rhs.wall;
}
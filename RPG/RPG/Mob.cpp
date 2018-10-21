#include "Mob.h"

#include "Scene.h"

Mob::Mob()
{
}

Mob::~Mob()
{
}

#include <iostream>

void Mob::move(const Vec2& d, Scene& scene)
{
	move(d.x, d.y, scene);
}

void Mob::move(float dx, float dy, Scene & scene)
{
	x += dx;
	float drx = copysignf(r, dx);
	float edge_x = x + drx;
	float overstep_x = edge_x - roundf(edge_x - dx);
	auto top = scene.getTile(edge_x, ceilf(y + r) - 1.0f);
	auto bot = scene.getTile(edge_x, y - r);
	x -= overstep_x * (top->solid | bot->solid);
	float slide_y = fabsf(overstep_x) * (bot->solid - top->solid);
	if (dx != 0.0f)
		slide_y *= 1.0f - fminf(1.0f, fabsf(dy / dx));
	y += slide_y;

	y += dy;
	float dry = copysignf(r, dy);
	float edge_y = y + dry;
	float overstep_y = edge_y - roundf(edge_y - dy);
	auto right = scene.getTile(ceilf(x + r) - 1.0f, edge_y);
	auto left = scene.getTile(x - r, edge_y);
	y -= overstep_y * (right->solid | left->solid);
	float slide_x = fabsf(overstep_y) * (left->solid - right->solid);
	if (dy != 0.0f)
		slide_x *= 1.0f - fminf(1.0f, fabsf(dx / dy));
	x += slide_x;
}

void Mob::moveHor(float dx, Scene& scene)
{
	x += dx;
	float drx = copysignf(r, dx);
	float edge = x + drx;
	float overstep = edge - roundf(edge - dx);
	auto top = scene.getTile(edge, ceilf(y + r) - 1.0f);
	auto bot = scene.getTile(edge, y - r);
	x -= overstep * (top->solid | bot->solid);
}

void Mob::moveVer(float dy, Scene& scene)
{
	y += dy;
	float dry = copysignf(r, dy);
	float edge = y + dry;
	float overstep = edge - roundf(edge - dy);
	auto right = scene.getTile(ceilf(x + r) - 1.0f, edge);
	auto left = scene.getTile(x - r, edge);
	y -= overstep * (right->solid | left->solid);
}
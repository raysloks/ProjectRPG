#include "Mob.h"

#include "Scene.h"

Mob::Mob()
{
}

Mob::~Mob()
{
}

void Mob::move_velocity(const Vec3& d, Scene& scene)
{
	float dx = d.x;
	float dy = d.y;
	float dz = d.z;

	x += dx;
	float drx = copysignf(r, dx);
	float edge_x = x + drx;
	float overstep_x = edge_x - roundf(edge_x - dx);
	auto pypz = scene.getTile(edge_x, ceilf(y + r) - 1.0f, ceilf(z + r) - 1.0f);
	auto pynz = scene.getTile(edge_x, ceilf(y + r) - 1.0f, z - r);
	auto nypz = scene.getTile(edge_x, y - r, ceilf(z + r) - 1.0f);
	auto nynz = scene.getTile(edge_x, y - r, z - r);
	auto solid_x = pypz.solid | pynz.solid | nypz.solid | nynz.solid;
	x -= overstep_x * solid_x;
	v.x -= v.x * solid_x;

	y += dy;
	float dry = copysignf(r, dy);
	float edge_y = y + dry;
	float overstep_y = edge_y - roundf(edge_y - dy);
	auto pxpz = scene.getTile(ceilf(x + r) - 1.0f, edge_y, ceilf(z + r) - 1.0f);
	auto pxnz = scene.getTile(ceilf(x + r) - 1.0f, edge_y, z - r);
	auto nxpz = scene.getTile(x - r, edge_y, ceilf(z + r) - 1.0f);
	auto nxnz = scene.getTile(x - r, edge_y, z - r);
	auto solid_y = pxpz.solid | pxnz.solid | nxpz.solid | nxnz.solid;
	y -= overstep_y * solid_y;
	v.y -= v.y * solid_y;

	z += dz;
	float drz = copysignf(r, dz);
	float edge_z = z + drz;
	float overstep_z = edge_z - roundf(edge_z - dz);
	auto pxpy = scene.getTile(ceilf(x + r) - 1.0f, ceilf(y + r) - 1.0f, edge_z);
	auto pxny = scene.getTile(ceilf(x + r) - 1.0f, y - r, edge_z);
	auto nxpy = scene.getTile(x - r, ceilf(y + r) - 1.0f, edge_z);
	auto nxny = scene.getTile(x - r, y - r, edge_z);
	auto solid_z = pxpy.solid | pxny.solid | nxpy.solid | nxny.solid;
	land = solid_z;
	z -= overstep_z * solid_z;
	v.z -= v.z * solid_z;
}

void Mob::move(const Vec3& d, Scene& scene)
{
	move(d.x, d.y, d.z, scene);
}

void Mob::move(float dx, float dy, float dz, Scene& scene)
{
	x += dx;
	float drx = copysignf(r, dx);
	float edge_x = x + drx;
	float overstep_x = edge_x - roundf(edge_x - dx);
	auto pypz = scene.getTile(edge_x, ceilf(y + r) - 1.0f, ceilf(z + r) - 1.0f);
	auto pynz = scene.getTile(edge_x, ceilf(y + r) - 1.0f, z - r);
	auto nypz = scene.getTile(edge_x, y - r, ceilf(z + r) - 1.0f);
	auto nynz = scene.getTile(edge_x, y - r, z - r);
	auto solid_x = pypz.solid | pynz.solid | nypz.solid | nynz.solid;
	x -= overstep_x * solid_x;

	y += dy;
	float dry = copysignf(r, dy);
	float edge_y = y + dry;
	float overstep_y = edge_y - roundf(edge_y - dy);
	auto pxpz = scene.getTile(ceilf(x + r) - 1.0f, edge_y, ceilf(z + r) - 1.0f);
	auto pxnz = scene.getTile(ceilf(x + r) - 1.0f, edge_y, z - r);
	auto nxpz = scene.getTile(x - r, edge_y, ceilf(z + r) - 1.0f);
	auto nxnz = scene.getTile(x - r, edge_y, z - r);
	auto solid_y = pxpz.solid | pxnz.solid | nxpz.solid | nxnz.solid;
	y -= overstep_y * solid_y;

	z += dz;
	float drz = copysignf(r, dz);
	float edge_z = z + drz;
	float overstep_z = edge_z - roundf(edge_z - dz);
	auto pxpy = scene.getTile(ceilf(x + r) - 1.0f, ceilf(y + r) - 1.0f, edge_z);
	auto pxny = scene.getTile(ceilf(x + r) - 1.0f, y - r, edge_z);
	auto nxpy = scene.getTile(x - r, ceilf(y + r) - 1.0f, edge_z);
	auto nxny = scene.getTile(x - r, y - r, edge_z);
	auto solid_z = pxpy.solid | pxny.solid | nxpy.solid | nxny.solid;
	z -= overstep_z * solid_z;
}
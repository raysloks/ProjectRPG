#ifndef MESH_H
#define MESH_H

#include "Resource.h"
#include "Vec3.h"
#include "Vec2.h"
#include <vector>
#include <map>
#include "SkeletalAnimation.h"

class RenderSetup;

class Vertex
{
public:
	Vertex(const Vec3& pos);

	Vec3 p, n;

	std::map<int, float> w;
};

class Face
{
public:
	Face(int va, int vb, int vc);

	void flip(void);

	int a, b, c;
	int ta, tb, tc;
	Vec3 n;
};

class VBO
{
public:
	VBO(void);

	unsigned int v, t, n;
};

class Texture;

class Mesh
	: public Resource
{
public:
	Mesh(void);
	Mesh(instream& is);
	Mesh(const Mesh& mesh);
	~Mesh(void);

	void render(RenderSetup& rs);//const;
	void render(RenderSetup& rs, const Pose& pose);//const;

	void transform(const Matrix4& mtrx, Mesh * mesh);//const;
	void getPose(const Pose& pose, Mesh * mesh);//const;

	void slotTexture(int slot, const std::shared_ptr<Texture>& texture);

	void addVBO(size_t set, Vec3 * v_data, Vec3 * n_data, Vec2 * t_data, Matrix4 mtrx);
	void buildVBO(void);

	std::vector<Vertex> vert;
	std::vector<std::vector<Face>> sets;
	std::vector<std::shared_ptr<Texture>> tex;
	std::vector<Vec2> uv;

	std::vector<VBO> vbos;
	bool vbo_latest;
};

#endif
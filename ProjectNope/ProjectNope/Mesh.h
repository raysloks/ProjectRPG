#ifndef MESH_H
#define MESH_H

#include "Resource.h"
#include "Vec3.h"
#include "Vec2.h"
#include <vector>
#include <map>
#include "SkeletalAnimation.h"

class RenderSetup;
class MaterialList;

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
	Face(unsigned short a, unsigned short b, unsigned short c);

	unsigned short a, b, c;
};

class VertexStruct
{
public:
	VertexStruct(const std::string& attribute, size_t size, bool normalize, size_t stride, size_t offset);
	~VertexStruct();

	void refresh(RenderSetup& rs);

	void enable();
	void specify();
	void disable();

	std::string attribute_name;
	int attribute_id;

	size_t size, stride, offset;
	bool normalize;
};

class VBO
{
public:
	VBO();
	~VBO();

	void addBuffer();
	void addVertexStruct(const VertexStruct& vs);

	void draw(RenderSetup& rs);

	std::vector<std::pair<unsigned int, std::vector<VertexStruct>>> buffers;
	size_t nIndices;
	std::map<unsigned int, unsigned int> vaos;
};

class FaceSet
{
public:
	std::vector<Face> vertices;
	std::vector<Face> uv_points;
	size_t nTextures;
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

	void render(RenderSetup& rs, MaterialList& mats);//const;?

	void transform(const Matrix4& mtrx, Mesh * mesh);//const;
	void getPose(const Pose& pose, Mesh * mesh);//const;

	void addVBO(size_t set, Vec3 * v_data, Vec3 * n_data, Vec2 * t_data, Matrix4 mtrx);
	void buildVBO(void);

	std::vector<Vertex> vert;
	std::vector<FaceSet> sets;
	std::vector<Vec2> uv;

	std::vector<std::shared_ptr<VBO>> vbos;
	bool vbo_latest;
};

#endif
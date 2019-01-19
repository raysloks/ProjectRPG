#ifndef MESH_H
#define MESH_H

#include "Resource.h"
#include "Vec3.h"
#include "Vec2.h"
#include <vector>
#include <map>
#include "SkeletalAnimation.h"
#include "MaterialList.h"

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
	Face(uint32_t a, uint32_t b, uint32_t c);

	uint32_t a, b, c;
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

	void bind(RenderSetup& rs);

	void draw(RenderSetup& rs);
	void draw_instanced(RenderSetup& rs, unsigned int nInstances);

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
	Material default_material;
};

class Texture;

class Mesh
	: public Resource
{
public:
	Mesh(void);
	Mesh(instream& is, const std::string& path);
	Mesh(const Mesh& mesh);
	~Mesh(void);

	void subdivide();
	void toSphere(float radius);

	void render(RenderSetup& rs, MaterialList& mats);//const;?
	void render_skinned(RenderSetup& rs, MaterialList& mats, const Pose& pose);
	void render_instanced(RenderSetup& rs, MaterialList& mats, unsigned int nInstances);

	void transform(const Matrix4& mtrx, Mesh * mesh);//const;
	void getPose(const Pose& pose, Mesh * mesh);//const;

	void addVBO(size_t set, Vec3 * v_data, Vec3 * n_data, Vec2 * t_data, Matrix4 mtrx);
	void buildVBO(void);

	void bind(const Pose& pose);

	std::vector<Vertex> vert;
	std::vector<FaceSet> sets;
	std::vector<Vec2> uv;

	std::vector<std::shared_ptr<VBO>> vbos;
	bool vbo_latest;
};

#endif
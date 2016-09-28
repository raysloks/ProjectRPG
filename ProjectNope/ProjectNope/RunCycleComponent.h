#ifndef RUN_CYCLE_COMPONENT_H
#define RUN_CYCLE_COMPONENT_H

#include "Component.h"

#include <memory>
#include <vector>

#include "GraphicsComponent.h"

class FrameBuffer;
class ShaderProgram;

class Texture;
class Mesh;

class RunCycleComponent :
	public Component
{
public:
	RunCycleComponent(const std::string& pointer);
	RunCycleComponent(instream& is, bool full);
	~RunCycleComponent(void);

	std::shared_ptr<Mesh> constructMeshFromDepth(const std::vector<float>& d, bool inverse = false);
	std::shared_ptr<Mesh> constructMeshFromDepth(const std::vector<float>& near, const std::vector<float>& far);

	void tick(float dTime);

	int file_format;

	float t;

	float play_speed;

	float view_plane;

	Matrix4 camera_mat;

	float cutoff;
	float diff;

	unsigned short w;
	unsigned short h;

	std::string pointer;
	std::vector<std::string> files;

	std::vector<std::vector<float>> depth;
	std::vector<std::vector<Vec3>> pos;
	std::vector<std::vector<float>> blob;

	std::vector<std::shared_ptr<Texture>> textures;
	std::vector<std::shared_ptr<Mesh>> meshes;
	
	std::shared_ptr<FrameBuffer> buffer;
	std::shared_ptr<ShaderProgram> analyze_program;

	GraphicsComponent * g;

	static const AutoSerialFactory<RunCycleComponent> _factory;
};

#endif
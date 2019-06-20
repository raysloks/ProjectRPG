#ifndef GRAPHICS_COMPONENT_H
#define GRAPHICS_COMPONENT_H

#include "Component.h"
#include "DecoratorContainer.h"
#include "ShaderMod.h"
#include "SkeletalAnimation.h"
#include "GlobalPosition.h"
#include "Mesh.h"

class PositionComponent;
class RenderSetup;

class PreparedVBO
{
public:
	GlobalPosition p;
	Matrix4 mtrx;
	VBO vbo;
};

typedef std::pair<int, std::string> render_priority;

class GraphicsComponent :
	public Component
{
public:
	GraphicsComponent(bool dynamic = true, uint32_t tag = 0);
	GraphicsComponent(instream& is);
	~GraphicsComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void writeLog(outstream& os, const std::shared_ptr<ClientData>& client);
	void readLog(instream& is);

	void pre_frame(float dTime);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, const std::shared_ptr<ClientData>& client) const;
	void write_to(outstream& os) const;

	void invalidate(void);

	static AutoSerialFactory<GraphicsComponent, Component> _factory;

	DecoratorContainer decs;

	bool dynamic;
	uint32_t tag;

	PositionComponent * p;

	Vec3 angle; // temp

	Pose * pose;

	void render(RenderSetup& rs);

	static thread_local std::vector<GraphicsComponent*> all;

	static void prep(RenderSetup& rs);
	static void render_all(RenderSetup& rs);
};

#endif
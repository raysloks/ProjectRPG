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
	GraphicsComponent(bool dynamic = true);
	GraphicsComponent(instream& is, bool full);
	~GraphicsComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void pre_frame(float dTime);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	void invalidate(void);

	static const AutoSerialFactory<GraphicsComponent> _factory;

	DecoratorContainer decs;

	bool dynamic;

	GlobalPosition * p;

	Pose * pose;

	void render(RenderSetup& rs);

	static std::vector<GraphicsComponent*> all;

	static void prep(RenderSetup& rs);
	static void render_all(RenderSetup& rs);
};

#endif
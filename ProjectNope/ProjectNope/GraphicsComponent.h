#ifndef GRAPHICS_COMPONENT_H
#define GRAPHICS_COMPONENT_H

#include "Component.h"
#include "DecoratorContainer.h"
#include "ShaderMod.h"
#include "SkeletalAnimation.h"
#include "GlobalPosition.h"

class PositionComponent;
class RenderSetup;

class PreparedVBO
{
public:
	GlobalPosition p;
	Matrix4 mtrx;
	unsigned int v, t, n, size;
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

	void frame(float dTime);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	void invalidate(void);

	static const AutoSerialFactory<GraphicsComponent> _factory;

	DecoratorContainer decs;

	bool dynamic;

	std::map<int, ShaderMod> mod;

	GlobalPosition * p;

	Pose pose;

	void render(RenderSetup& rs);

	static std::vector<GraphicsComponent*> all;
	static std::map<render_priority, std::pair<unsigned int, std::vector<PreparedVBO>>> standard_dynamic;
	static std::vector<GraphicsComponent*> custom_dynamic;
	static std::map<render_priority, std::pair<unsigned int, PreparedVBO>> standard_static;
	static std::set<render_priority> complete;

	static void prep(void);
	static void render_all(RenderSetup& rs);
};

#endif
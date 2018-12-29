#ifndef LINE_COMPONENT_H
#define LINE_COMPONENT_H

#include "Component.h"

#include "Vec3.h"
#include "Matrix4.h"
#include "Matrix3.h"

#include <set>

class GlobalPosition;
class PositionComponent;
class RenderSetup;

class LineComponent :
	public Component
{
public:
	LineComponent(void);
	LineComponent(instream& is, bool full);
	~LineComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void tick(float dTime);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, ClientData& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	static AutoSerialFactory<LineComponent, Component> _factory;

	std::vector<std::pair<Vec3, Vec3>> lines;
	std::vector<std::pair<Vec3, Vec3>> color;

	GlobalPosition * p=0;

	Matrix4 transform;
	Matrix3 col_mat;

	void render(RenderSetup& rs);

	static std::set<LineComponent*> all;
	static int mode; // 0 = x-ray; 1 = fully occluded; 2 = semi-transparent

	static void render_all(RenderSetup& rs);
};

#endif
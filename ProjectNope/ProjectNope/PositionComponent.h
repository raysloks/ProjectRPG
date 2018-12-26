#ifndef POSITION_COMPONENT_H
#define POSITION_COMPONENT_H

#include "Component.h"
#include "GlobalPosition.h"

class PositionComponent :
	public Component
{
public:
	PositionComponent(void);
	explicit PositionComponent(const GlobalPosition& position);
	PositionComponent(instream& is, bool full);
	~PositionComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	void update(void);

	ASF_H(PositionComponent, Component)

	GlobalPosition p;

	size_t sync;

	bool send;
};

#endif
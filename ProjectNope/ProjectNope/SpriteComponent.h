#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include "Component.h"

class SpriteComponent :
	public Component
{
public:
	SpriteComponent(void);
	SpriteComponent(instream& is);
	~SpriteComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void pre_frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, const std::shared_ptr<ClientData>& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, const std::shared_ptr<ClientData>& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, const std::shared_ptr<ClientData>& client) const;
	void write_to(outstream& os) const;

	static AutoSerialFactory<SpriteComponent, Component> _factory;
};

#endif
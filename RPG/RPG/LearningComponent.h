#ifndef LEARNING_COMPONENT_H
#define LEARNING_COMPONENT_H

#include "Component.h"

class LearningComponent :
	public Component
{
public:
	LearningComponent(void);
	LearningComponent(instream& is, bool full);
	~LearningComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void pre_frame(float dTime);
	void post_frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, ClientData& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	static const AutoSerialFactory<LearningComponent> _factory;
};

#endif
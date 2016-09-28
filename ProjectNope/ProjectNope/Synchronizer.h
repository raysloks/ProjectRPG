#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

#include <vector>
#include <tuple>

#include "streams.h"

class Synchronizer
{
public:
	Synchronizer(float limit = 0.0f);
	~Synchronizer(void);

	void blend(Synchronizer& other);

	void writeLog(outstream& os) const;
	void readLog(instream& is);

	std::vector<float> val;
	std::vector<std::pair<float, float*>> var;
};

#endif
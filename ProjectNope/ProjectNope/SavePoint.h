#ifndef SAVE_POINT_H
#define SAVE_POINT_H

#include "StaticProp.h"

class SavePoint
	: public StaticProp
{
public:
	SavePoint(const GlobalPosition& pos, const std::string& mesh, const std::string& texture, const Matrix3& matrix = Matrix3());
	SavePoint(const GlobalPosition& pos, const std::string& mesh, const std::vector<std::string>& textures, const Matrix3& matrix = Matrix3());
	SavePoint(instream& is, bool full);
	virtual ~SavePoint(void);

	virtual void tick(float dTime);
	virtual void render(const GlobalPosition& origin);

	static const AutoSerialFactory<SavePoint> _factory_SavePoint;
};

#endif
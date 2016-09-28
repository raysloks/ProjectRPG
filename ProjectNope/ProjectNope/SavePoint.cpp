#include "SavePoint.h"
#include "GUIObject.h"
#include "Resource.h"
#include "Mesh.h"
#include "Texture.h"

#include <iostream>

const AutoSerialFactory<SavePoint> SavePoint::_factory_SavePoint("SavePoint");

SavePoint::SavePoint(const GlobalPosition& pos, const std::string& mesh, const std::string& texture, const Matrix3& matrix) : StaticProp(pos, mesh, texture, matrix), Serializable(_factory_SavePoint.id)
{
}

SavePoint::SavePoint(const GlobalPosition& pos, const std::string& mesh, const std::vector<std::string>& textures, const Matrix3& matrix) : StaticProp(pos, mesh, textures, matrix), Serializable(_factory_SavePoint.id)
{
}

SavePoint::SavePoint(instream& is, bool full) : StaticProp(is, full), Serializable(_factory_SavePoint.id)
{
}

SavePoint::~SavePoint(void)
{
}

void SavePoint::tick(float dTime)
{
	StaticProp::tick(dTime);
}

void SavePoint::render(const GlobalPosition& origin)
{
	StaticProp::render(origin);
}

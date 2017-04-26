#include "ShaderMod.h"

ShaderMod::ShaderMod()
{
}

ShaderMod::ShaderMod(const std::shared_ptr<ShaderProgram>& s) : shader(s)
{
}

ShaderMod::ShaderMod(const std::function<void(const std::shared_ptr<ShaderProgram>&)>& m) : mod(m)
{
}

ShaderMod::ShaderMod(const std::shared_ptr<ShaderProgram>& s, const std::function<void(const std::shared_ptr<ShaderProgram>&)>& m) : shader(s), mod(m)
{
}

ShaderMod::~ShaderMod()
{
}

bool ShaderMod::operator==(const ShaderMod& rhs)
{
	return shader == rhs.shader && !mod && !rhs.mod;
}

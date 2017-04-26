#ifndef SHADER_MOD_H
#define SHADER_MOD_H

#include <functional>
#include <memory>

class ShaderProgram;

class ShaderMod
{
public:
	ShaderMod();
	ShaderMod(const std::shared_ptr<ShaderProgram>& shader);
	ShaderMod(const std::function<void(const std::shared_ptr<ShaderProgram>&)>& mod);
	ShaderMod(const std::shared_ptr<ShaderProgram>& shader, const std::function<void(const std::shared_ptr<ShaderProgram>&)>& mod);
	~ShaderMod();

	bool operator==(const ShaderMod& rhs);

	std::shared_ptr<ShaderProgram> shader;
	std::function<void(const std::shared_ptr<ShaderProgram>&)> mod;
};

#endif
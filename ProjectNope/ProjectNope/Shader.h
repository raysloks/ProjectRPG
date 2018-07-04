#ifndef SHADER_H
#define SHADER_H

#include <memory>
#include <string>

enum ShaderType
{
	SHADER_VERTEX,
	SHADER_GEOMETRY,
	SHADER_FRAGMENT
};

class Shader
{
public:
	Shader(ShaderType type);
	Shader(const std::string& fname, ShaderType type);
	~Shader(void);

	static std::shared_ptr<Shader> get(const std::string& fname, ShaderType type);

	void complete();
	void refresh();

	std::string fname;
	ShaderType type;

	unsigned int gl_shader;
};

#endif
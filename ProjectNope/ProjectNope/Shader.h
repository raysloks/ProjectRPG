#ifndef SHADER_H
#define SHADER_H

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

	void complete();
	void refresh();

	std::string fname;
	ShaderType type;

	unsigned int gl_shader;
};

#endif
#include "Shader.h"

#include <fstream>
#include <iostream>
#include <map>

#include "Resource.h"
#include "StringResource.h"
#include "GUIObject.h"

std::map<std::string, std::shared_ptr<Shader>> shaders;

Shader::Shader(ShaderType type) : type(type), gl_shader(0)
{
}

Shader::Shader(const std::string& fname, ShaderType type) : fname(fname), type(type), gl_shader(0)
{
	Resource::load(fname);
}

Shader::~Shader(void)
{
}

std::shared_ptr<Shader> Shader::get(const std::string& fname, ShaderType type)
{
	auto i = shaders.find(fname);
	if (i != shaders.end())
		return i->second;
	auto shader = std::make_shared<Shader>(fname, type);
	shaders.insert(std::make_pair(fname, shader));
	return shader;
}

void Shader::complete()
{
	Resource::load(fname, { "block" });
}

void Shader::refresh()
{
	if (gl_shader == 0)
	{
		auto file = Resource::get<StringResource>(fname);
		if (file != nullptr)
		{
			switch (type)
			{
			case SHADER_VERTEX:
				gl_shader = glCreateShader(GL_VERTEX_SHADER);
				break;
			case SHADER_GEOMETRY:
				gl_shader = glCreateShader(GL_GEOMETRY_SHADER);
				break;
			case SHADER_FRAGMENT:
				gl_shader = glCreateShader(GL_FRAGMENT_SHADER);
			}
			const char * constbuffer = file->string.c_str();
			const GLint len = file->string.size();
			glShaderSource(gl_shader, 1, &constbuffer, &len);
			glCompileShader(gl_shader);

			GLint isCompiled = 0;
			glGetShaderiv(gl_shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint l = 0;
				glGetShaderiv(gl_shader, GL_INFO_LOG_LENGTH, &l);

				if (l>0) {
					std::vector<GLchar> log(l);
					glGetShaderInfoLog(gl_shader, l, &l, &log[0]);

					std::cout << &log[0] << std::endl;
				}
			}
		}
	}
}
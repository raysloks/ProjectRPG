#include "ShaderProgram.h"
#include <fstream>
#include <vector>

#include <iostream>

GLint ShaderProgram::GetUniformLocation(const std::string& name)
{
	if (uloc.find(name)==uloc.end())
		return uloc[name] = glGetUniformLocation(prog, name.c_str());
	return uloc[name];
}

GLint ShaderProgram::GetAttributeLocation(const std::string& name)
{
	if (aloc.find(name)==aloc.end())
		return aloc[name] = glGetAttribLocation(prog, name.c_str());
	return aloc[name];
}

ShaderProgram::ShaderProgram(void)
{
	prog = glCreateProgram();
}

ShaderProgram::ShaderProgram(std::shared_ptr<Shader>& v, std::shared_ptr<Shader>& f)
{
	prog = glCreateProgram();
	vert = v;
	frag = f;
	glAttachShader(prog, vert->shader);
	glAttachShader(prog, frag->shader);
	glLinkProgram(prog);

	GLint isLinked = 0;
	glGetProgramiv(prog, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint l = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &l);
 
		if (l>0) {
			std::vector<GLchar> log(l);
			glGetProgramInfoLog(prog, l, &l, &log[0]);

			std::cout << &log[0] << std::endl;
		}
	}
}

ShaderProgram::ShaderProgram(std::shared_ptr<Shader>& v, std::shared_ptr<Shader>& g, std::shared_ptr<Shader>& f)
{
	prog = glCreateProgram();
	geom = g;
	vert = v;
	frag = f;
	glAttachShader(prog, geom->shader);
	glAttachShader(prog, vert->shader);
	glAttachShader(prog, frag->shader);
	glLinkProgram(prog);

	GLint isLinked = 0;
	glGetProgramiv(prog, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint l = 0;
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &l);

		if (l>0) {
			std::vector<GLchar> log(l);
			glGetProgramInfoLog(prog, l, &l, &log[0]);

			std::cout << &log[0] << std::endl;
		}
	}
}

ShaderProgram::ShaderProgram(const std::string& fvert, const std::string& ffrag)
{
	prog = glCreateProgram();
	vert.reset(new Shader(fvert, GL_VERTEX_SHADER));
	frag.reset(new Shader(ffrag, GL_FRAGMENT_SHADER));
	glAttachShader(prog, vert->shader);
	glAttachShader(prog, frag->shader);
	glLinkProgram(prog);

	GLint isLinked = 0;
	glGetProgramiv(prog, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint l = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &l);
 
		if (l>0) {
			std::vector<GLchar> log(l);
			glGetProgramInfoLog(prog, l, &l, &log[0]);

			std::cout << &log[0] << std::endl;
		}
	}
}

ShaderProgram::~ShaderProgram(void)
{
	glDeleteProgram(prog);
}

void ShaderProgram::Uniform1f(const std::string& name, float f)
{
	glUniform1f(GetUniformLocation(name), f);
}

void ShaderProgram::Uniform2f(const std::string& name, float f1, float f2)
{
	glUniform2f(GetUniformLocation(name), f1, f2);
}

void ShaderProgram::Uniform3f(const std::string& name, float f1, float f2, float f3)
{
	glUniform3f(GetUniformLocation(name), f1, f2, f3);
}

void ShaderProgram::Uniform4f(const std::string& name, float f1, float f2, float f3, float f4)
{
	glUniform4f(GetUniformLocation(name), f1, f2, f3, f4);
}

void ShaderProgram::Uniform1fv(const std::string& name, const std::vector<float>& fv)
{
	glUniform1fv(GetUniformLocation(name), fv.size(), &fv[0]);
}

void ShaderProgram::Uniform2fv(const std::string& name, const std::vector<float>& fv)
{
	glUniform2fv(GetUniformLocation(name), fv.size(), &fv[0]);
}

void ShaderProgram::Uniform3fv(const std::string& name, const std::vector<float>& fv)
{
	glUniform3fv(GetUniformLocation(name), fv.size(), &fv[0]);
}

void ShaderProgram::Uniform4fv(const std::string& name, const std::vector<float>& fv)
{
	glUniform4fv(GetUniformLocation(name), fv.size(), &fv[0]);
}

void ShaderProgram::UniformMatrix3fv(const std::string& name, const float* fv)
{
	glUniformMatrix3fv(GetUniformLocation(name), 1, false, fv);
}

void ShaderProgram::UniformMatrix4fv(const std::string& name, const float* fv)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, false, fv);
}

void ShaderProgram::Uniform1i(const std::string& name, int i)
{
	glUniform1i(GetUniformLocation(name), i);
}

void ShaderProgram::Uniform2i(const std::string& name, int i1, int i2)
{
	glUniform2i(GetUniformLocation(name), i1, i2);
}

void ShaderProgram::Uniform3i(const std::string& name, int i1, int i2, int i3)
{
	glUniform3i(GetUniformLocation(name), i1, i2, i3);
}

void ShaderProgram::Uniform4i(const std::string& name, int i1, int i2, int i3, int i4)
{
	glUniform4i(GetUniformLocation(name), i1, i2, i3, i4);
}

void ShaderProgram::Attribute1f(const std::string& name, float f)
{
	glVertexAttrib1f(GetAttributeLocation(name), f);
}

void ShaderProgram::Attribute2f(const std::string& name, float f1, float f2)
{
	glVertexAttrib2f(GetAttributeLocation(name), f1, f2);
}

void ShaderProgram::Attribute3f(const std::string& name, float f1, float f2, float f3)
{
	glVertexAttrib3f(GetAttributeLocation(name), f1, f2, f3);
}

void ShaderProgram::Attribute4f(const std::string& name, float f1, float f2, float f3, float f4)
{
	glVertexAttrib4f(GetAttributeLocation(name), f1, f2, f3, f4);
}
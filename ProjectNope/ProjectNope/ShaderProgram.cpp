#include "ShaderProgram.h"

#include <fstream>
#include <vector>

#include <iostream>

#include "GUIObject.h"

GLint ShaderProgram::GetUniformLocation(const std::string& name)
{
	if (gl_uloc.find(name)==gl_uloc.end())
		return gl_uloc[name] = glGetUniformLocation(gl_program, name.c_str());
	return gl_uloc[name];
}

GLint ShaderProgram::GetAttributeLocation(const std::string& name)
{
	if (gl_aloc.find(name)==gl_aloc.end())
		return gl_aloc[name] = glGetAttribLocation(gl_program, name.c_str());
	return gl_aloc[name];
}

ShaderProgram::ShaderProgram(void) : gl_program(0)
{
}

ShaderProgram::ShaderProgram(std::shared_ptr<Shader>& v, std::shared_ptr<Shader>& f) : vert(v), frag(f), gl_program(0)
{
}

ShaderProgram::ShaderProgram(std::shared_ptr<Shader>& v, std::shared_ptr<Shader>& g, std::shared_ptr<Shader>& f) : vert(v), geom(g), frag(f), gl_program(0)
{
}

ShaderProgram::ShaderProgram(const std::string& fvert, const std::string& ffrag) : gl_program(0)
{
	vert = std::make_shared<Shader>(fvert, SHADER_VERTEX);
	frag = std::make_shared<Shader>(ffrag, SHADER_FRAGMENT);
}

ShaderProgram::ShaderProgram(const std::string & fvert, const std::string & fgeom, const std::string & ffrag) : gl_program(0)
{
	vert = std::make_shared<Shader>(fvert, SHADER_VERTEX);
	geom = std::make_shared<Shader>(fgeom, SHADER_GEOMETRY);
	frag = std::make_shared<Shader>(ffrag, SHADER_FRAGMENT);
}

ShaderProgram::~ShaderProgram(void)
{
	if (gl_program != 0)
		glDeleteProgram(gl_program);
}

bool ShaderProgram::IsReady()
{
	if (gl_program == 0)
	{
		bool ready = true;
		if (vert != nullptr)
		{
			vert->refresh();
			if (vert->gl_shader == 0)
				ready = false;
		}
		if (geom != nullptr)
		{
			geom->refresh();
			if (geom->gl_shader == 0)
				ready = false;
		}
		if (frag != nullptr)
		{
			frag->refresh();
			if (frag->gl_shader == 0)
				ready = false;
		}

		if (ready)
		{
			gl_program = glCreateProgram();
			if (vert != nullptr)
				glAttachShader(gl_program, vert->gl_shader);
			if (geom != nullptr)
				glAttachShader(gl_program, geom->gl_shader);
			if (frag != nullptr)
				glAttachShader(gl_program, frag->gl_shader);
			glLinkProgram(gl_program);

			gl_uloc.clear();
			gl_aloc.clear();

			GLint isLinked = 0;
			glGetProgramiv(gl_program, GL_LINK_STATUS, &isLinked);
			if (isLinked == GL_FALSE)
			{
				GLint l = 0;
				glGetProgramiv(gl_program, GL_INFO_LOG_LENGTH, &l);

				if (l>0) {
					std::vector<GLchar> log(l);
					glGetProgramInfoLog(gl_program, l, &l, &log[0]);

					std::cout << &log[0] << std::endl;
				}
			}
		}
	}
	return gl_program != 0;
}

bool ShaderProgram::Use()
{
	bool ready = IsReady();
	glUseProgram(gl_program);
	return ready;
}

void ShaderProgram::Uniform(const std::string& name, float f)
{
	glUniform1f(GetUniformLocation(name), f);
}

void ShaderProgram::Uniform(const std::string& name, float f1, float f2)
{
	glUniform2f(GetUniformLocation(name), f1, f2);
}

void ShaderProgram::Uniform(const std::string & name, const Vec2 & v)
{
	glUniform2f(GetUniformLocation(name), v.x, v.y);
}

void ShaderProgram::Uniform(const std::string& name, float f1, float f2, float f3)
{
	glUniform3f(GetUniformLocation(name), f1, f2, f3);
}

void ShaderProgram::Uniform(const std::string & name, const Vec3 & v)
{
	glUniform3f(GetUniformLocation(name), v.x, v.y, v.z);
}

void ShaderProgram::Uniform(const std::string& name, float f1, float f2, float f3, float f4)
{
	glUniform4f(GetUniformLocation(name), f1, f2, f3, f4);
}

void ShaderProgram::Uniform(const std::string & name, const Vec3 & v, float f4)
{
	glUniform4f(GetUniformLocation(name), v.x, v.y, v.z, f4);
}

void ShaderProgram::Uniform(const std::string & name, const Vec4 & v)
{
	glUniform4f(GetUniformLocation(name), v.x, v.y, v.z, v.w);
}

void ShaderProgram::Uniform1fv(const std::string& name, const std::vector<float>& fv)
{
	glUniform1fv(GetUniformLocation(name), fv.size(), fv.data());
}

void ShaderProgram::Uniform2fv(const std::string& name, const std::vector<Vec2>& fv)
{
	glUniform2fv(GetUniformLocation(name), fv.size(), (float*)fv.data());
}

void ShaderProgram::Uniform2fv(const std::string& name, const std::vector<float>& fv)
{
	glUniform2fv(GetUniformLocation(name), fv.size() / 2, fv.data());
}


void ShaderProgram::Uniform3fv(const std::string& name, const std::vector<Vec3>& fv)
{
	glUniform3fv(GetUniformLocation(name), fv.size(), (float*)fv.data());
}

void ShaderProgram::Uniform3fv(const std::string& name, const std::vector<float>& fv)
{
	glUniform3fv(GetUniformLocation(name), fv.size() / 3, fv.data());
}

void ShaderProgram::Uniform4fv(const std::string& name, const std::vector<Vec4>& fv)
{
	glUniform4fv(GetUniformLocation(name), fv.size(), (float*)fv.data());
}

void ShaderProgram::Uniform4fv(const std::string& name, const std::vector<float>& fv)
{
	glUniform4fv(GetUniformLocation(name), fv.size() / 4, fv.data());
}

void ShaderProgram::UniformMatrix3f(const std::string& name, const float* fv)
{
	glUniformMatrix3fv(GetUniformLocation(name), 1, false, fv);
}

void ShaderProgram::UniformMatrix4f(const std::string& name, const float* fv)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, false, fv);
}

void ShaderProgram::UniformMatrix3fv(const std::string& name, const std::vector<Matrix3>& fv)
{
	glUniformMatrix3fv(GetUniformLocation(name), fv.size(), false, (float*)fv.data());
}

void ShaderProgram::UniformMatrix4fv(const std::string& name, const std::vector<Matrix4>& fv)
{
	glUniformMatrix4fv(GetUniformLocation(name), fv.size(), false, (float*)fv.data());
}

void ShaderProgram::Uniform(const std::string& name, int i)
{
	glUniform1i(GetUniformLocation(name), i);
}

void ShaderProgram::Uniform(const std::string& name, int i1, int i2)
{
	glUniform2i(GetUniformLocation(name), i1, i2);
}

void ShaderProgram::Uniform(const std::string& name, int i1, int i2, int i3)
{
	glUniform3i(GetUniformLocation(name), i1, i2, i3);
}

void ShaderProgram::Uniform(const std::string& name, int i1, int i2, int i3, int i4)
{
	glUniform4i(GetUniformLocation(name), i1, i2, i3, i4);
}

void ShaderProgram::Attribute(const std::string& name, float f)
{
	glVertexAttrib1f(GetAttributeLocation(name), f);
}

void ShaderProgram::Attribute(const std::string& name, float f1, float f2)
{
	glVertexAttrib2f(GetAttributeLocation(name), f1, f2);
}

void ShaderProgram::Attribute(const std::string& name, float f1, float f2, float f3)
{
	glVertexAttrib3f(GetAttributeLocation(name), f1, f2, f3);
}

void ShaderProgram::Attribute(const std::string& name, float f1, float f2, float f3, float f4)
{
	glVertexAttrib4f(GetAttributeLocation(name), f1, f2, f3, f4);
}
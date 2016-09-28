#include "Shader.h"
#include <map>

#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

class ShaderProgram
{
public:
	ShaderProgram(void);
	ShaderProgram(std::shared_ptr<Shader>& vert, std::shared_ptr<Shader>& frag);
	ShaderProgram(std::shared_ptr<Shader>& vert, std::shared_ptr<Shader>& geom, std::shared_ptr<Shader>& frag);
	ShaderProgram(const std::string& fvert, const std::string& ffrag);
	~ShaderProgram(void);

	GLint GetUniformLocation(const std::string& name);

	void Uniform1f(const std::string& name, float f);
	void Uniform2f(const std::string& name, float f1, float f2);
	void Uniform3f(const std::string& name, float f1, float f2, float f3);
	void Uniform4f(const std::string& name, float f1, float f2, float f3, float f4);
	void Uniform1fv(const std::string& name, const std::vector<float>& fv);
	void Uniform2fv(const std::string& name, const std::vector<float>& fv);
	void Uniform3fv(const std::string& name, const std::vector<float>& fv);
	void Uniform4fv(const std::string& name, const std::vector<float>& fv);
	void UniformMatrix3fv(const std::string& name, const float* fv);
	void UniformMatrix4fv(const std::string& name, const float* fv);
	void Uniform1i(const std::string& name, int i);
	void Uniform2i(const std::string& name, int i1, int i2);
	void Uniform3i(const std::string& name, int i1, int i2, int i3);
	void Uniform4i(const std::string& name, int i1, int i2, int i3, int i4);

	GLint GetAttributeLocation(const std::string& name);

	void Attribute1f(const std::string& name, float f);
	void Attribute2f(const std::string& name, float f1, float f2);
	void Attribute3f(const std::string& name, float f1, float f2, float f3);
	void Attribute4f(const std::string& name, float f1, float f2, float f3, float f4);

	GLuint prog;
	std::map<std::string, GLint> uloc, aloc;
	std::shared_ptr<Shader> geom, vert, frag;
};

#endif
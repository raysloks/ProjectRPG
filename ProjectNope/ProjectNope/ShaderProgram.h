#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <memory>
#include <vector>
#include <map>

#include "Shader.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix3.h"
#include "Matrix4.h"

class ShaderProgram
{
public:
	ShaderProgram(void);
	ShaderProgram(std::shared_ptr<Shader>& vert, std::shared_ptr<Shader>& frag);
	ShaderProgram(std::shared_ptr<Shader>& vert, std::shared_ptr<Shader>& geom, std::shared_ptr<Shader>& frag);
	ShaderProgram(const std::string& fvert, const std::string& ffrag);
	ShaderProgram(const std::string& fvert, const std::string& fgeom, const std::string& ffrag);
	~ShaderProgram(void);

	bool IsReady();
	bool Use();

	int GetUniformLocation(const std::string& name);

	void Uniform(const std::string& name, float f);
	void Uniform(const std::string& name, float f1, float f2);
	void Uniform(const std::string& name, const Vec2& v);
	void Uniform(const std::string& name, float f1, float f2, float f3);
	void Uniform(const std::string& name, const Vec3& v);
	void Uniform(const std::string& name, float f1, float f2, float f3, float f4);
	void Uniform(const std::string& name, const Vec3& v, float f4);
	void Uniform(const std::string& name, const Vec4& v);
	void Uniform1fv(const std::string& name, size_t count, const float* fv);
	void Uniform1fv(const std::string& name, const std::vector<float>& fv);
	void Uniform2fv(const std::string& name, const std::vector<Vec2>& fv);
	void Uniform2fv(const std::string& name, const std::vector<float>& fv);
	void Uniform3fv(const std::string& name, const std::vector<Vec3>& fv);
	void Uniform3fv(const std::string& name, const std::vector<float>& fv);
	void Uniform4fv(const std::string& name, const std::vector<Vec4>& fv);
	void Uniform4fv(const std::string& name, const std::vector<float>& fv);
	void UniformMatrix3f(const std::string& name, const float* fv);
	void UniformMatrix4f(const std::string& name, const float* fv);
	void UniformMatrix3fv(const std::string& name, size_t count, const Matrix3* fv);
	void UniformMatrix4fv(const std::string& name, size_t count, const Matrix4* fv);
	void UniformMatrix3fv(const std::string& name, const std::vector<Matrix3>& fv);
	void UniformMatrix4fv(const std::string& name, const std::vector<Matrix4>& fv);
	void Uniform(const std::string& name, int i);
	void Uniform(const std::string& name, int i1, int i2);
	void Uniform(const std::string& name, int i1, int i2, int i3);
	void Uniform(const std::string& name, int i1, int i2, int i3, int i4);

	int GetAttributeLocation(const std::string& name);

	void Attribute(const std::string& name, float f);
	void Attribute(const std::string& name, float f1, float f2);
	void Attribute(const std::string& name, float f1, float f2, float f3);
	void Attribute(const std::string& name, float f1, float f2, float f3, float f4);

	std::shared_ptr<Shader> geom, vert, frag;

	unsigned int gl_program;
	std::map<std::string, int> gl_uloc, gl_aloc;
};

#endif
#ifndef SCRIPT_COMPILE_H
#define SCRIPT_COMPILE_H

#include "ScriptScope.h"
#include "ScriptCompileMemoryTarget.h"
#include "ScriptFunctionPrototype.h"

#include <vector>

class ScriptCompile
{
public:
	ScriptCompile();
	~ScriptCompile();

	void BeginScope();
	void EndScope();

	template <typename T>
	T GetAt(size_t position)
	{
		T ret;
		auto prev = ss.tellg();
		ss.seekg(position);
		ss.read((char*)&ret, sizeof(T));
		ss.seekg(prev);
		return ret;
	}

	template <typename T>
	void SetAt(size_t position, const T& rhs)
	{
		auto prev = ss.tellp();
		ss.seekp(position);
		ss.write((char*)&rhs, sizeof(T));
		ss.seekp(prev);
	}

	void Insert(size_t start, off_t size);

	void PushVariable(const std::string& name, ScriptTypeData type);
	void PushVariable(const std::string& name, ScriptTypeData type, ScriptCompileMemoryTarget target);
	void PushVariable(const std::string& name, ScriptTypeData type, uint32_t value);
	ScriptVariableData GetVariable(const std::string& name);

	std::stringstream ss;

	ScriptFunctionPrototype proto;

	std::vector<ScriptScope> scope;
	size_t stack;

	ScriptCompileMemoryTarget target;

	std::vector<size_t> rel8;
	std::vector<size_t> rel32;
};

#endif
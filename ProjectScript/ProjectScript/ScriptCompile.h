#ifndef SCRIPT_COMPILE_H
#define SCRIPT_COMPILE_H

#include "ScriptScope.h"
#include "ScriptCompileMemoryTarget.h"
#include "ScriptFunctionPrototype.h"
#include "ScriptClassData.h"
#include "ScriptAssemblyHelper.h"
#include "ScriptFunctionCompileData.h"
#include "ScriptLinkData.h"

#include <vector>
#include <memory>
#include <unordered_map>
#include <set>

class ScriptCompile
{
public:
	ScriptCompile(void * base_pointer);
	~ScriptCompile();

	void BeginFunction();
	void EndFunction();

	void BeginScope();
	void EndScope();
	void EndScopeAggresive();

	void Compile();
	void Link();

	void AddStackDependant();
	void AddStackDependantNegative();

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

	void Cut(size_t start, size_t size);
	void Adjust(size_t start, off_t size);

	void PushVariable(const std::string& name);
	void PushVariable(const std::string& name, ScriptTypeData type);
	void PushVariable(const std::string& name, ScriptTypeData type, ScriptCompileMemoryTarget target);
	void PushVariable(const std::string& name, ScriptTypeData type, int32_t value);

	void AddParameterStack(const std::string& name, ScriptTypeData type, int32_t offset);
	void AddParameterRegister(const std::string& name, ScriptTypeData type, uint8_t reg);

	ScriptVariableData GetVariable(const std::string& name);

	void SetClass(const std::string& name);

	bool IsBusy(uint8_t reg);
	bool IsFree(uint8_t reg);
	void SetBusy(uint8_t reg);
	void SetFree(uint8_t reg);

	std::unique_ptr<ScriptFunctionPrototype> proto;
	std::shared_ptr<ScriptClassData> current_class;

	std::unordered_map<std::string, std::shared_ptr<ScriptClassData>> classes;
	std::vector<ScriptFunctionCompileData> function_code;

	std::stringstream ss;
	void * base_pointer;

	ScriptAssemblyHelper sasm;

	std::vector<ScriptScope> scope;
	size_t stack, max_stack;
	std::vector<size_t> stack_dependants;
	std::vector<size_t> stack_dependants_negative;

	ScriptCompileMemoryTarget target;

	std::set<uint8_t> busy_registers;

	std::vector<size_t> rel8;
	std::vector<size_t> rel32;
	std::vector<ScriptLinkData> links;
};

#endif
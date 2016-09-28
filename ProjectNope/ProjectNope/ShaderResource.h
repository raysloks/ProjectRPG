#include "Resource.h"
#include "ShaderProgram.h"

#ifndef SHADER_RESOURCE_H
#define SHADER_RESOURCE_H

class ShaderResource
{
public:
	ShaderResource(const std::string& fname);
	~ShaderResource(void);

	std::shared_ptr<ShaderProgram> get(void);

	std::string file_name;
	std::shared_ptr<ShaderProgram> program;
};

#endif
#include "GUIObject.h"
#include <string>

#ifndef SHADER_H
#define SHADER_H

class Shader
{
public:
	Shader(GLenum type);
	Shader(const std::string& fname, GLenum type);
	~Shader(void);

	GLuint shader;
};

#endif
#include "Shader.h"
#include <fstream>

#include <iostream>

using namespace std;

Shader::Shader(GLenum type)
{
	shader = glCreateShader(type);
}

Shader::Shader(const std::string& fname, GLenum type)
{
	shader = glCreateShader(type);/*
	ifstream f(fname, ios::in|ios::binary|ios::ate);
	int size = f.tellg();
	char * buffer = new char [size+1];
	f.seekg (0, ios::beg);
	f.read (buffer, size);
	f.close();
	buffer[size]=0;*/
	const char * constbuffer = &fname[0];
	const GLint len = fname.size();
	glShaderSource(shader, 1, &constbuffer, &len);
	glCompileShader(shader);

	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint l = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &l);
 
		if (l>0) {
			std::vector<GLchar> log(l);
			glGetShaderInfoLog(shader, l, &l, &log[0]);

			cout << &log[0] << endl;
		}
	}
}

Shader::~Shader(void)
{
}
#include "ShaderResource.h"

ShaderResource::ShaderResource(const std::string& fname) : file_name(fname)
{
}

ShaderResource::~ShaderResource(void)
{
}

std::shared_ptr<ShaderProgram> ShaderResource::get(void)
{
	/*std::string * text = static_cast<std::string*>(Resource::load(file_name));
	if (text!=0)
	{
		std::string vst("#VERTEX_SHADER");
		auto posv = text->find(vst);
		std::string fst("#FRAGMENT_SHADER");
		auto posf = text->find(fst);
		std::shared_ptr<Shader> vs(new Shader(std::string(text->begin()+posv+vst.size(), text->begin()+posf), GL_VERTEX_SHADER));
		std::shared_ptr<Shader> fs(new Shader(std::string(text->begin()+posf+fst.size(), text->end()), GL_FRAGMENT_SHADER));
		program.reset(new ShaderProgram(vs, fs));
	}
	if (program!=0)
		return program;*/
	return 0;
}
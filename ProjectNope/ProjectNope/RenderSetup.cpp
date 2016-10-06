#include "RenderSetup.h"

RenderSetup::RenderSetup()
{
	tmp_use_default_state = true;
}

RenderSetup::~RenderSetup()
{
}

void RenderSetup::pushMod(const ShaderMod& mod)
{
	mod_stack.push_back(mod);
}

void RenderSetup::popMod(void)
{
	mod_stack.pop_back();
}

void RenderSetup::applyMods(void)
{
	auto previous_program = current_program;
	for (auto i=mod_stack.begin();i!=mod_stack.end();++i)
		if (i->shader!=0)
			current_program = i->shader;
	if (current_program!=previous_program)
		glUseProgram(current_program->prog);
	for (auto i=mod_stack.begin();i!=mod_stack.end();++i)
		if (i->mod!=0)
			i->mod(current_program);
}

void RenderSetup::pushTransform(void)
{
	transform_stack.push_back(transform);
}

void RenderSetup::popTransform(void)
{
	transform = transform_stack.back();
	transform_stack.pop_back();
}

void RenderSetup::setTransform(const Matrix4& mat)
{
	transform = mat;
}

Matrix4 RenderSetup::getTransform(void)
{
	return transform;
}

void RenderSetup::addTransform(const Matrix4& mat)
{
	transform = mat * transform;
}
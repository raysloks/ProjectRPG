#ifndef RENDER_SETUP_H
#define RENDER_SETUP_H

#include "ShaderProgram.h"
#include "ShaderMod.h"

#include "Matrix4.h"

#include "GlobalPosition.h"

#include "Vec2.h"

class RenderSetup
{
public:
	RenderSetup();
	~RenderSetup();

	void pushMod(const ShaderMod& mod);
	void popMod(void);

	void applyMods(void);

	void pushTransform(void);
	void popTransform(void);

	void setTransform(const Matrix4& mat);
	Matrix4 getTransform(void);
	void addTransform(const Matrix4& mat);

	int pass;

	Vec2 size;

	GlobalPosition origin;

	Matrix4 transform, view;

	std::vector<Matrix4> transform_stack;
	std::vector<ShaderMod> mod_stack;
	std::shared_ptr<ShaderProgram> current_program;

	bool tmp_use_default_state; // TODO make better
};

#endif
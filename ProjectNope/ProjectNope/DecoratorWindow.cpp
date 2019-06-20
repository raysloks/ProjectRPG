#include "DecoratorWindow.h"

#include "RenderSetup.h"

#include "GUIObject.h"

#include "Quaternion.h"

DecoratorWindow::DecoratorWindow()
{
}

DecoratorWindow::~DecoratorWindow()
{
}

void DecoratorWindow::render(RenderSetup& rs)
{
	if (dec)
	{
		rs.pushTransform();
		rs.addTransform(Matrix4::Translation((min + max) * 0.5f));
		rs.addTransform(Matrix4::Scale(fminf(size.x, size.y)));
		rs.addTransform(Quaternion(1.0f, Vec3(-1.0f, 1.0f, 0.0f).Normalize()));

		ShaderMod mod3d(ShaderProgram::Get("data/gfill_vert.txt", "data/gfill_frag.txt"), [](const std::shared_ptr<ShaderProgram> & prog) {
			prog->Uniform("light", Vec3(1.0f, -1.0f, 1.0f).Normalize());
			});

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		rs.pushMod(mod3d);
		dec->render(rs);
		rs.popMod();

		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		rs.popTransform();
	}
	
	Window::render(rs);
}
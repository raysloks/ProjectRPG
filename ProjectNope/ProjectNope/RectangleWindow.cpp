#include "RectangleWindow.h"
#include "RenderSetup.h"

#include "Texture.h"

RectangleWindow::RectangleWindow(float px, float py, float width, float height)
{
	min_offset = Vec2(px, py);
	max_offset = min_offset + Vec2(width, height);
	color = Vec4(0.003f, 0.003f, 0.003f, 0.85f);
	background = "data/assets/white.tga";
}

RectangleWindow::~RectangleWindow(void)
{
	Window::~Window();
}

void RectangleWindow::render(RenderSetup& rs)
{
	if (!enabled)
		return;

	rs.pushTransform();
	rs.addTransform(Matrix4::Translation(min));

	Vec4 color_final = color;
	ShaderMod mod(ShaderProgram::Get("data/gui_vert.txt", "data/gui_frag.txt"), [&color_final](const std::shared_ptr<ShaderProgram>& prog) {
		prog->Uniform("color", color_final);
	});

	rs.pushMod(mod);

	auto image = Resource::get<Texture>(background);
	auto border = Resource::get<Texture>("data/assets/white.tga");
	if (image)
	{
		image->render(rs, size);

		color_final = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		if (cursor)
			color_final = Vec4(1.0f);

		border->render(rs, Vec2(size.x, 1.0f));
		border->render(rs, Vec2(1.0f, size.y));

		rs.addTransform(Matrix4::Translation(Vec2(size.x - 1.0f, 0.0f)));
		border->render(rs, Vec2(1.0f, size.y));
		rs.addTransform(Matrix4::Translation(Vec2(1.0f - size.x, size.y - 1.0f)));
		border->render(rs, Vec2(size.x, 1.0f));
	}

	rs.popMod();

	rs.popTransform();

	Window::render(rs);
}
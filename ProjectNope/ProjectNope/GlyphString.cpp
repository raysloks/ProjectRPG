#include "GlyphString.h"

#include "Glyph.h"
#include "Writing.h"
#include "RenderSetup.h"
#include "Mesh.h"
#include "GUIObject.h"
#include "GlyphAtlas.h"
#include "Texture.h"

#include "Profiler.h"

GlyphString::GlyphString(void)
{
}

GlyphString::GlyphString(const std::string& s) : string(s)
{
}

GlyphString::~GlyphString(void)
{
}

void GlyphString::render(RenderSetup& rs)
{
	rs.pushTransform();

	TimeslotC(string_render);

	VBO vbo;
	vbo.addBuffer();
	vbo.addVertexStruct(VertexStruct("pos", 2, false, 0, 0));
	vbo.addBuffer();
	vbo.addVertexStruct(VertexStruct("uv", 2, false, 0, 0));

	float * p_data = new float[string.size() * 12];
	float * t_data = new float[string.size() * 12];
	float * p_fill = p_data;
	float * t_fill = t_data;

	size_t n_chars = 0;

	float offset_x = 0.0f;
	float offset_y = 0.0f;

	auto fr = Resource::get<FontResource>(font);
	if (fr != nullptr)
	{
		{
			TimeslotC(data_construction);

			for (std::string::const_iterator c = string.cbegin(); c != string.cend();)
			{
				unsigned int code_point = Writing::getCodePoint(c, string.cend());

				if (code_point == '\n')
				{
					offset_x = 0;
					offset_y += size_t(y_size * 1.15f);
				}
				else
				{
					auto glyph = fr->getGlyph(code_point, x_size, y_size);
					if (glyph != nullptr)
					{
						glyph->insert(p_fill, t_fill, offset_x, offset_y);

						p_fill += 12;
						t_fill += 12;

						auto bitmap = (FT_BitmapGlyph)glyph->ftBitmap;

						offset_x += bitmap->root.advance.x >> 16;
						offset_y += bitmap->root.advance.y >> 16;

						++n_chars;
					}
				}
			}
		}

		{
			TimeslotC(buffer_push);

			glBindBuffer(GL_ARRAY_BUFFER, vbo.buffers[0].first);
			glBufferData(GL_ARRAY_BUFFER, n_chars * 12 * sizeof(float), p_data, GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, vbo.buffers[1].first);
			glBufferData(GL_ARRAY_BUFFER, n_chars * 12 * sizeof(float), t_data, GL_STATIC_DRAW);
		}

		delete[] p_data;
		delete[] t_data;

		vbo.nIndices = n_chars * 6;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fr->atlas->texture->texid);

		rs.addTransform(Matrix4::Translation(Vec3(offset_x, offset_y, 0.0f) * offset));

		rs.applyMods();

		vbo.draw(rs);

		rs.addTransform(Matrix4::Translation(Vec3(offset_x, offset_y, 0.0f)));
	}
}
#include "Writing.h"
#include "GUIObject.h"
#include "Vec2.h"
#include "Texture.h"
#include "Resource.h"
#include "Glyph.h"
#include "RenderSetup.h"

std::string font;
float r, g, b, a;
size_t x_size, y_size;

void Writing::setFont(const std::string& name)
{
	font = name;
}

void Writing::setColor(float nr, float ng, float nb, float na)
{
	r = nr;
	g = ng;
	b = nb;
	a = na;
}

void Writing::setSize(size_t size)
{
	x_size = 0;
	y_size = size;
}

void Writing::setSize(size_t xs, size_t ys)
{
	x_size = xs;
	y_size = ys;
}

std::pair<std::string::iterator, std::string::iterator> Writing::getRange(const std::string::iterator& begin, const std::string::iterator& pos, const std::string::iterator& end)
{
	std::string::iterator front = pos;
	while (front!=begin)
	{
		unsigned char byte = reinterpret_cast<const unsigned char&>(*front);
		if (byte<128 || byte>191)
			break;
		front--;
	}

	std::string::iterator back = pos;
	while (back!=end)
	{
		back++;
		if (back==end)
			break;
		unsigned char byte = reinterpret_cast<const unsigned char&>(*back);
		if (byte<128 || byte>191)
			break;
	}

	return std::make_pair(front, back);
}

unsigned int Writing::getCodePoint(std::string::const_iterator& c, const std::string::const_iterator& end)
{
	if (c!=end)
	{
		unsigned char byte = reinterpret_cast<const unsigned char&>(*c);
		unsigned int heading;
		for (heading = 0;heading<7;++heading)
		{
			unsigned char check = byte<<heading;
			if (check<128)
				break;
		}
		unsigned int offset = (max(heading, 1)-1)*6;

		unsigned int code_point = 0;
		code_point |= (reinterpret_cast<const unsigned char&>(*c++) & 127) << offset;
		for (int i=1;i<heading;++i)
		{
			if (c!=end)
			{
				offset -= 6;
				code_point |= (reinterpret_cast<const unsigned char&>(*c++) & 63) << offset;
			}
		}
		return code_point;
	}
	c++;
	return 0;
}

size_t Writing::getLength(const std::string& text)
{
	size_t len = 0;
	for (std::string::const_iterator c=text.cbegin();c!=text.cend();)
	{
		getCodePoint(c, text.cend());
		len++;
	}
	return len;
}

unsigned int getCodePage(unsigned int code_point)
{
	return code_point/256;
}

std::tuple<Vec2, Vec2, Vec2, Vec2> getMapping(unsigned int code_point, unsigned int code_page)
{
	return std::make_tuple(Vec2((code_point%256%16), (code_point%256/16))/16, Vec2(1.0f/16.0f, 1.0f/16.0f), Vec2(0.0f, 0.0f), Vec2(1.0f, 0.0f));
}

void loadBitmap(unsigned int code_page)
{
	std::shared_ptr<Texture> tex = Resource::get<Texture>(font + "/" + std::to_string((_ULonglong)code_page) + ".tga");
	if (tex!=0) {
		glBindTexture(GL_TEXTURE_2D, tex->getGLTexID());
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

std::string::iterator Writing::erase(std::string& str, const std::string::iterator& p)
{
	auto range = getRange(str.begin(), p, str.end());
	return str.erase(range.first, range.second);
}

#include "GlyphString.h"

#include "Profiler.h"

void Writing::render(const std::string& text, RenderSetup& rs)
{
	ShaderMod mod(nullptr, [](const std::shared_ptr<ShaderProgram>& prog) {
		prog->Uniform("color", Vec4(r, g, b, a));
	});

	rs.pushMod(mod);

	GlyphString gs(text);
	gs.font = font;
	gs.x_size = x_size;
	gs.y_size = y_size;
	gs.render(rs);

	rs.popMod();

	/*auto font = Resource::get<FontResource>("data/assets/fonts/Lora-Regular.ttf");
	if (font!=0)
	{
		if (texid==0)
		{
			if (glyph==0)
				glyph = std::make_shared<Glyph>(font->getFace(), 196);
			if (glyph!=0 && bm==0)
				bm = std::shared_ptr<Glyph>(glyph->getBitmap());

			if (bm!=0)
			{
				auto bitmap = (FT_BitmapGlyph)bm->ftGlyph;

				glGenTextures(1, &texid);
				glBindTexture(GL_TEXTURE_2D, texid);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, bitmap->bitmap.width, bitmap->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap->bitmap.buffer);
			}
		}
		if (texid!=0)
		{
			auto bitmap = (FT_BitmapGlyph)bm->ftGlyph;

			glBindTexture(GL_TEXTURE_2D, texid);

			glBegin(GL_QUADS);

			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(bitmap->bitmap.width, 0.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(bitmap->bitmap.width, bitmap->bitmap.rows);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(0.0f, bitmap->bitmap.rows);

			glEnd();
		}
	}*/
	
	/*unsigned int current_page = 0;
	loadBitmap(current_page);

	glBegin(GL_QUADS);
	Vec2 offset;
	std::tuple<Vec2, Vec2, Vec2, Vec2> c_mapping;
	for (std::string::const_iterator c=text.cbegin();c!=text.cend();)
	{
		unsigned int code_point = getCodePoint(c, text.cend());
		unsigned int new_page = getCodePage(code_point);

		if (new_page!=current_page)
		{
			glEnd();
			loadBitmap(new_page);
			current_page = new_page;
			glBegin(GL_QUADS);
		}

		c_mapping = getMapping(code_point, current_page);
		glTexCoord2f(std::get<0>(c_mapping).x, 1.0f-std::get<0>(c_mapping).y);
		glVertex2f(offset.x+std::get<2>(c_mapping).x, offset.y+std::get<2>(c_mapping).y);
		glTexCoord2f(std::get<0>(c_mapping).x+std::get<1>(c_mapping).x, 1.0f-std::get<0>(c_mapping).y);
		glVertex2f(offset.x+std::get<2>(c_mapping).x+c_size.x, offset.y+std::get<2>(c_mapping).y);
		glTexCoord2f(std::get<0>(c_mapping).x+std::get<1>(c_mapping).x, 1.0f-std::get<0>(c_mapping).y-std::get<1>(c_mapping).y);
		glVertex2f(offset.x+std::get<2>(c_mapping).x+c_size.x, offset.y+std::get<2>(c_mapping).y+c_size.y);
		glTexCoord2f(std::get<0>(c_mapping).x, 1.0f-std::get<0>(c_mapping).y-std::get<1>(c_mapping).y);
		glVertex2f(offset.x+std::get<2>(c_mapping).x, offset.y+std::get<2>(c_mapping).y+c_size.y);
		offset.x += std::get<3>(c_mapping).x*c_size.x;
		offset.y += std::get<3>(c_mapping).y*c_size.y;
	}
	glEnd();*/
}
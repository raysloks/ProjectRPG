#include "LineComponent.h"

#include "NewEntity.h"
#include "GUIObject.h"

#include "PositionComponent.h"

#include "RenderSetup.h"

const AutoSerialFactory<LineComponent> LineComponent::_factory("LineComponent");

std::set<LineComponent*> LineComponent::all;

int LineComponent::mode = 2;

LineComponent::LineComponent(void) : Serializable(_factory.id)
{
	all.insert(this);
	col_mat *= 0.5f;
	p = 0;
}

LineComponent::LineComponent(instream& is, bool full) : Serializable(_factory.id)
{
	all.insert(this);
	col_mat *= 0.5f;
	p = 0;
}

LineComponent::~LineComponent(void)
{
	if (all.find(this)!=all.end())
		all.erase(this);
}

void LineComponent::connect(NewEntity * pEntity, bool authority)
{
}

void LineComponent::disconnect(void)
{
}

void LineComponent::tick(float dTime)
{
}

void LineComponent::writeLog(outstream& os, ClientData& client)
{
}

void LineComponent::readLog(instream& is)
{
}

void LineComponent::writeLog(outstream& os)
{
}

void LineComponent::readLog(instream& is, ClientData& client)
{
}

void LineComponent::interpolate(Component * pComponent, float fWeight)
{
	LineComponent * pLineComponent = dynamic_cast<LineComponent*>(pComponent);
	if (pLineComponent!=0) {
		lines = pLineComponent->lines;
	}
}

void LineComponent::write_to(outstream& os, ClientData& client) const
{
}

void LineComponent::write_to(outstream& os) const
{
}

void LineComponent::render(RenderSetup& rs)
{
	rs.pushTransform();

	GlobalPosition offset = -rs.origin;
	if (p==0) {
		PositionComponent * pc = entity->getComponent<PositionComponent>();
		if (pc!=0)
			p = &pc->p;
	}
	if (p!=0)
		offset += *p;
	Vec3 pos = offset;
	rs.addTransform(Matrix4::Translation(pos));

	rs.addTransform(transform);

	glLoadIdentity();
	glMultMatrixf(rs.getTransform().data);

	glBegin(GL_LINES);

	if (rs.pass==2)
	{
		for (int i=0;i<lines.size() && i<color.size();++i)
		{
			glColor3f(color[i].first.x, color[i].first.y, color[i].first.z);
			glVertex3f(lines[i].first.x, lines[i].first.y, lines[i].first.z);
			glColor3f(color[i].second.x, color[i].second.y, color[i].second.z);
			glVertex3f(lines[i].second.x, lines[i].second.y, lines[i].second.z);
		}
	}
	else
	{
		for (int i=0;i<lines.size() && i<color.size();++i)
		{
			Vec3 col = color[i].first * col_mat;
			glColor3f(col.x, col.y, col.z);
			glVertex3f(lines[i].first.x, lines[i].first.y, lines[i].first.z);
			col = color[i].second * col_mat;
			glColor3f(col.x, col.y, col.z);
			glVertex3f(lines[i].second.x, lines[i].second.y, lines[i].second.z);
		}
	}

	glEnd();

	rs.popTransform();
}

void LineComponent::render_all(RenderSetup& rs)
{
	glDisable(GL_LIGHTING);

	for (auto i=all.begin();i!=all.end();++i)
		(*i)->render(rs);
}
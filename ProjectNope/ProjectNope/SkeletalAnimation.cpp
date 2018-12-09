#include "SkeletalAnimation.h"
#include <iostream>
#include "GUIObject.h"

Action::Action(float l) : length(l)
{
}

Action::Action(void) : length(0.0f)
{
}

Action::~Action(void)
{
}

Pose::Pose(void)
{
}

Pose::Pose(const Pose& pose)
{
	bones = std::vector<Bone>(pose.bones);
	rest = pose.rest;
	for (int i=0;i<bones.size();++i)
	{
		for (int j=0;j<pose.bones.size();++j)
		{
			if (bones[i].parent==&pose.bones[j])
				bones[i].parent=&bones[j];
		}
		/*std::cout << &pose.bones[i] << " " << pose.bones[i].parent << std::endl;
		std::cout << &bones[i] << " " << bones[i].parent << std::endl;*/
	}
}

Pose::~Pose(void)
{
}

Pose& Pose::operator=(const Pose& pose)
{
	bones = std::vector<Bone>(pose.bones.size());
	rest = pose.rest;
	for (int i=0;i<bones.size();++i)
	{
		bones[i] = pose.bones[i];
	}
	for (int i=0;i<bones.size();++i)
	{
		for (int j=0;j<pose.bones.size();++j)
		{
			if (bones[i].parent==&pose.bones[j])
				bones[i].parent=&bones[j];
		}
		/*std::cout << &pose.bones[i] << " " << pose.bones[i].parent << std::endl;
		std::cout << &bones[i] << " " << bones[i].parent << std::endl;*/
	}
	return *this;
}

void Pose::debug_render(void) const
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);

	for (int i=0;i<bones.size();++i)
	{
		Vec3 p;
		if (bones[i].parent!=0)
		{
			glColor3f(1.0f, 1.0f, 1.0f);
			p *= bones[i].getTransform();
			glVertex3f(p.x, p.y, p.z);
			p = Vec3()*bones[i].parent->getTransform();
			glVertex3f(p.x, p.y, p.z);
		}

		glColor3f(1.0f, 0.0f, 0.0f);
		p = Vec3();
		p *= bones[i].getTransform();
		glVertex3f(p.x, p.y, p.z);
		p = Vec3(1.0f, 0.0f, 0.0f)*bones[i].getTransform();
		glVertex3f(p.x, p.y, p.z);

		glColor3f(0.0f, 1.0f, 0.0f);
		p = Vec3();
		p *= bones[i].getTransform();
		glVertex3f(p.x, p.y, p.z);
		p = Vec3(0.0f, 1.0f, 0.0f)*bones[i].getTransform();
		glVertex3f(p.x, p.y, p.z);

		glColor3f(0.0f, 0.0f, 1.0f);
		p = Vec3();
		p *= bones[i].getTransform();
		glVertex3f(p.x, p.y, p.z);
		p = Vec3(0.0f, 0.0f, 1.0f)*bones[i].getTransform();
		glVertex3f(p.x, p.y, p.z);
	}

	glEnd();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

#include <iostream>

Pose& Pose::interpolate(const Pose& pose, float fWeight)
{
	float l;
	if (bones.size()==pose.bones.size())
	{
		for (int i=0;i<bones.size();++i)
		{
			bones[i].transform = bones[i].transform*(1.0f-fWeight)+pose.bones[i].transform*fWeight;
			for (int k=0;k<3;++k)
			{
				l = 0.0f;
				for (int j=0;j<3;++j)
					l += bones[i].transform.mtrx[j][k]*bones[i].transform.mtrx[j][k];
				l = sqrt(l);
				for (int j=0;j<3;++j)
					bones[i].transform.mtrx[j][k] /= l;
			}
		}
		update();
	}
	return *this;
}

Pose& Pose::add(const Pose& pose)
{
	if (bones.size()==pose.bones.size())
	{
		Matrix3 trot3;
		Matrix4 trot4;
		for (int i=0;i<bones.size();++i)
		{
			for (int x=0;x<3;++x) {
				for (int y=0;y<3;++y) {
					trot3.mtrx[y][x] = rest->bones[i].transform.mtrx[y][x];
				}
			}
			trot3 = trot3.Inverse();
			for (int x=0;x<3;++x) {
				for (int y=0;y<3;++y) {
					trot4.mtrx[y][x] = trot3.mtrx[y][x];
				}
			}
			bones[i].transform = bones[i].transform*Matrix4::Translation(Vec3(-rest->bones[i].transform.data[12], -rest->bones[i].transform.data[13], -rest->bones[i].transform.data[14]))*trot4*pose.bones[i].transform;
		}
		update();
	}
	return *this;
}

void Pose::update(void)
{
	transforms.resize(bones.size());
	for (size_t i = 0; i < bones.size(); ++i)
	{
		bones[i].total_transform = bones[i].getTransform();
		transforms[i] = rest->bones[i].total_inverse * bones[i].total_transform;
	}
}

outstream& operator<<(outstream& os, const Pose& pose)
{
	os << (uint32_t)pose.bones.size();
	for (int i=0;i<pose.bones.size();++i)
	{
		os << pose.bones[i].transform;
		int parent = -1;
		for (int j=0;j<pose.bones.size();++j)
		{
			//std::cout << i << " " << j << " " << &(pose.bones[i]) << " " << pose.bones[i].parent << " " << &(pose.bones[j]) << std::endl;
			if (pose.bones[i].parent==&(pose.bones[j])) {
				parent = j;
			}
		}
		os << parent;
	}
	return os;
}

instream& operator>>(instream& is, Pose& pose)
{
	uint32_t size;
	is >> size;
	pose.bones.resize(size);
	for (int i=0;i<size;++i)
	{
		int parent;
		is >> pose.bones[i].transform >> parent;
		if (parent>=0)
			pose.bones[i].parent = &pose.bones[parent];
		else
			pose.bones[i].parent = 0;
	}
	pose.rest = 0;
	return is;
}

Bone::Bone(void)
{
	parent = nullptr;
}

Bone::~Bone(void)
{
}

Matrix4 Bone::getTransform(void)const
{
	if (parent != nullptr)
		return transform*parent->getTransform();
	else
		return transform;
}

SkeletalAnimation::SkeletalAnimation(instream& is)
{
	armature.rest = &armature;
	uint32_t total_bones;
	is >> total_bones;
	std::vector<int> parents;
	std::vector<Matrix4> locals;
	for (int o=0;o<total_bones;++o)
	{
		int index, parent_index;
		std::string name;
		is >> index;
		is >> name;
		is >> parent_index;
		if (index<0)
			index = 0;//error in file
		if (index >= armature.bones.size())
			armature.bones.resize(index + 1);
		if (index >= parents.size())
			parents.resize(index + 1);
		if (index >= locals.size())
			locals.resize(index + 1);
		if (index >= names.size())
			names.resize(index + 1);
		parents[index] = parent_index;
		for (int i=0;i<4;++i) {
			for (int j=0;j<4;++j) {
				is >> locals[index].mtrx[j][i];
				//std::cout << locals[index].mtrx[j][i] << " ";
			}
			//std::cout << std::endl;
		}
		names[index] = name;
	}
	
	for (int o=0;o<armature.bones.size();++o)
	{
		if (parents[o]>=0) {
			armature.bones[o].parent = &armature.bones[parents[o]];

			Matrix4 m4 = locals[parents[o]];
			Matrix3 m3;
			for (int x=0;x<3;++x)
				for (int y=0;y<3;++y)
					m3.mtrx[x][y] = m4.mtrx[x][y];
			m3 = m3.Inverse();
			for (int x=0;x<3;++x)
				for (int y=0;y<3;++y)
					m4.mtrx[x][y] = m3.mtrx[x][y];
			armature.bones[o].transform = locals[o]*Matrix4::Translation(-Vec3(m4.data[12], m4.data[13], m4.data[14]));
			m4.data[12] = 0.0f;
			m4.data[13] = 0.0f;
			m4.data[14] = 0.0f;
			armature.bones[o].transform *= m4;
		} else {
			armature.bones[o].parent = 0;
			armature.bones[o].transform = locals[o];
		}
	}

	for (int i=0;i<armature.bones.size();++i)
	{
		armature.bones[i].total_transform = armature.bones[i].getTransform();
		armature.bones[i].total_inverse = armature.bones[i].total_transform.Inverse();
	}

	uint32_t total_actions;
	is >> total_actions;
	for (int o=0;o<total_actions;++o)
	{
		std::string action_name;
		is >> action_name;
		actions.insert(std::pair<std::string, Action>(action_name, Action()));
		Action * action = &actions.at(action_name);
		action->anim = this;
		uint32_t total_groups;
		is >> total_groups;
		for (int i=0;i<total_groups;++i)
		{
			int group_index, total_channels;
			is >> group_index >> total_channels;
			if (group_index >= action->keys.size())
				action->keys.resize(group_index+1);
			if (group_index >= action->props.size())
				action->props.resize(group_index + 1);
			for (int j=0;j<total_channels;++j)
			{
				unsigned char type;
				is >> type;
				if (type >= action->keys[group_index].size())
					action->keys[group_index].resize(type + 1);
				if (type >= 10)
				{
					unsigned char prop = type - 10;
					if (prop >= action->props[group_index].size())
						action->props[group_index].resize(prop + 1);
					is >> action->props[group_index][prop];
				}
				int total_keys;
				is >> total_keys;
				for (int k=0;k<total_keys;++k)
				{
					float x, y;
					is >> x >> y;
					if (x>action->length)
						action->length=x;
					action->keys[group_index][type].insert(std::pair<float, float>(x, y));
				}
			}
		}
	}

	compileActions(1.0f);
}

std::shared_ptr<Pose> SkeletalAnimation::getPose(float time, const Action& act) const
{
	std::shared_ptr<Pose> pose(new Pose());
	pose->bones = std::vector<Bone>(armature.bones);
	for (int i=0;i<act.keys.size();++i)
	{
		std::vector<float> values;
		values.resize(act.keys[i].size());
		for (int j=0;j<act.keys[i].size();++j)
		{
			bool has_value = false;
			for (std::map<float, float>::const_iterator it=act.keys[i][j].cbegin();it!=act.keys[i][j].cend();++it)
			{
				if (it->first>=time)
				{
					if (it==act.keys[i][j].begin()) {
						values[j]=it->second;
					} else {
						std::map<float, float>::const_iterator dit = it;
						--dit;
						values[j]=(dit->second*(it->first-time)+it->second*(time-dit->first))/(it->first-dit->first);
					}
					has_value = true;
					break;
				}
			}
			if (!has_value)
			{
				std::map<float, float>::const_iterator it=act.keys[i][j].cend();
				if (it!=act.keys[i][j].cbegin())
				{
					--it;
					values[j]=it->second;
				}
				else
				{
					values[j]=0.0f;
				}
			}
		}
		Matrix4 rot;
		Matrix4 pos;
		Matrix4 scl;
		if (values.size()>0)
		{
			Quaternion q(values[3], values[4], values[5], values[6]);
			q.Normalize();
			rot = Matrix4(q);//*armature.bones[i].transform;
			rot.data[3] = 0.0f;
			rot.data[7] = 0.0f;
			rot.data[11] = 0.0f;
			rot.data[12] = 0.0f;
			rot.data[13] = 0.0f;
			rot.data[14] = 0.0f;
			Matrix3 trot3;
			Matrix4 trot4;
			for (int x=0;x<3;++x) {
				for (int y=0;y<3;++y) {
					trot3.mtrx[y][x] = armature.bones[i].transform.mtrx[y][x];
				}
			}
			trot3 = trot3.Inverse();
			for (int x=0;x<3;++x) {
				for (int y=0;y<3;++y) {
					trot4.mtrx[y][x] = trot3.mtrx[y][x];
				}
			}
			pos = Matrix4::Translation(Vec3(values[0], values[1], values[2]));
			scl = Matrix4::Scale(Vec3(values[7], values[8], values[9]));
			auto it = act.props[i].begin();
			for (size_t j = 10; j < values.size() && it != act.props[i].end(); ++it, ++j)
			{
				pose->bones[i].props.insert(std::make_pair(*it, values[j]));
			}
		}
		pose->bones[i].transform = rot*pos*armature.bones[i].transform;
	}
	for (size_t i = 0; i < pose->bones.size(); i++)
	{
		pose->bones[i].parent = 0;
		for (size_t j = 0; j < armature.bones.size(); j++)
		{
			if (&(armature.bones[j])==armature.bones[i].parent)
				pose->bones[i].parent = &(pose->bones[j]);
		}
	}
	pose->rest = &armature;
	pose->update();
	return pose;
}

std::shared_ptr<Pose> SkeletalAnimation::getPose(float time, const std::string& action) const
{
	auto i = actions.find(action);
	if (i != actions.cend())
	{
		std::shared_ptr<Pose> pose = getPose(time, i->second);
		return pose;
	}
	return nullptr;
}

float SkeletalAnimation::getStart(const std::string& action) const
{
	auto i = actions.find(action);
	if (i != actions.cend())
	{
		return i->second.compiled_start;
	}
	return 0.0f;
}

float SkeletalAnimation::getEnd(const std::string& action) const
{
	auto i = actions.find(action);
	if (i != actions.cend())
	{
		return i->second.compiled_end;
	}
	return 0.0f;
}

float SkeletalAnimation::getLength(const std::string & action) const
{
	auto i = actions.find(action);
	if (i != actions.cend())
	{
		return i->second.length;
	}
	return 0.0f;
}

int SkeletalAnimation::getIndex(const std::string& name) const
{
	for (int i = 0; i < names.size(); ++i)
	{
		if (names[i].compare(name) == 0)
			return i;
	}
	return -1;
}

Matrix4 SkeletalAnimation::getMatrix(int bone_id, float frame) const
{
	if (bone_id >= armature.bones.size() || bone_id < 0)
		return Matrix4();

	frame -= 0.5f;
	int frame_integer = frame;
	int next_frame = std::ceilf(frame);

	if (frame_integer < 0)
		return armature.bones[bone_id].total_transform;
	if (next_frame >= frame_count)
		return armature.bones[bone_id].total_transform;

	Matrix4 matrix = compiled_actions[frame_integer * armature.bones.size() + bone_id];
	Matrix4 next_matrix = compiled_actions[next_frame * armature.bones.size() + bone_id];

	float weight = next_frame - frame;

	return armature.bones[bone_id].total_transform * (matrix * weight + next_matrix * (1.0f - weight));
}

float SkeletalAnimation::getProperty(const std::string& name, float frame) const
{
	auto it = compiled_props.find(name);
	if (it == compiled_props.end())
		return 0.0f;

	frame -= 0.5f;
	int frame_integer = frame;
	int next_frame = frame_integer + 1;

	if (frame_integer < 0)
		return 0.0f;
	if (next_frame >= it->second.size())
		return 0.0f;

	float prop = it->second[frame_integer];
	float next_prop = it->second[next_frame];

	float weight = next_frame - frame;

	return prop * weight + next_prop * (1.0f - weight);
}

void SkeletalAnimation::compileActions(float resolution)
{
	frame_count = 0;
	float total_t = 0.0f;
	for (auto& action : actions)
	{
		action.second.compiled_start = total_t + resolution * 0.5f;
		for (float t = 0.0f; t <= action.second.length; t += resolution)
		{
			total_t += resolution;
			++frame_count;
			auto pose = getPose(t, action.second);
			for (size_t i = 0; i < pose->bones.size(); ++i)
			{
				compiled_actions.push_back(pose->rest->bones[i].total_inverse * pose->bones[i].total_transform);
				if (i < action.second.props.size())
				{
					for (size_t j = 0; j < action.second.props[i].size(); ++j)
					{
						std::string prop_name = names[i] + "." + action.second.props[i][j];
						compiled_props[prop_name].resize(frame_count, 0.0f);
						compiled_props[prop_name][frame_count - 1] = pose->bones[i].props[action.second.props[i][j]];
					}
				}
			}
		}
		action.second.compiled_end = total_t - resolution * 0.5f;
	}
}

#include "Texture.h"

std::shared_ptr<Texture> SkeletalAnimation::getCompiledTexture(void)
{
	if (!compiled_texture && !compiled_actions.empty())
	{
		compiled_texture = std::make_shared<Texture>();

		glGenTextures(1, &compiled_texture->texid);
		glBindTexture(GL_TEXTURE_2D, compiled_texture->texid);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		compiled_texture->w = armature.bones.size() * 4;
		compiled_texture->h = compiled_actions.size() / armature.bones.size();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, compiled_texture->w, compiled_texture->h, 0, GL_RGBA, GL_FLOAT, compiled_actions.data());
	}
	return compiled_texture;
}

SkeletalAnimation::~SkeletalAnimation(void)
{
}
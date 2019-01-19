#ifndef SKELETAL_ANIMATION_H
#define SKELETAL_ANIMATION_H

#include "Vec3.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include <vector>
#include <map>
#include <string>

class Pose;
class Bone;
class SkeletalAnimation;

class Action
{
public:
	Action(void);
	Action(float length);
	~Action(void);

	void fill_cache();

	std::vector<std::vector<std::map<float, float>>> keys;
	std::vector<std::vector<std::string>> props;
	float length;

	std::vector<Pose> cache;

	float compiled_start, compiled_end;
	SkeletalAnimation * anim;
};

class Pose
{
public:
	Pose(void);
	Pose(const Pose& pose);
	~Pose(void);

	Pose& operator=(const Pose& pose);

	void debug_render(void)const;

	Pose& interpolate(const Pose& pose, float fWeight);
	Pose& add(const Pose& pose);

	void update(void);

	std::vector<Bone> bones;
	std::vector<Matrix4> transforms;
	const Pose * rest;
};

outstream& operator<<(outstream& os, const Pose& pose);
instream& operator>>(instream& is, Pose& pose);

class Bone
{
public:
	Bone(void);
	~Bone(void);

	Matrix4 getTransform(void)const;

	Vec3 position, scale;
	Quaternion rotation;
	Matrix4 local;
	Matrix4 total_transform;
	Matrix4 total_inverse;
	Bone* parent;

	std::map<std::string, float> props;
};

#include "Resource.h"

class Texture;

class SkeletalAnimation
	: public Resource
{
public:
	SkeletalAnimation(instream& is);
	~SkeletalAnimation(void);

	void getPose(float time, const Action& action, Pose& pose) const;
	void getPose(float time, const std::string& action, Pose& pose) const;
	Pose getPose(float time, const Action& action) const;
	Pose getPose(float time, const std::string& action) const;
	std::shared_ptr<Pose> getPoseRaw(float time, const Action& action) const;
	std::shared_ptr<Pose> getPoseRaw(float time, const std::string& action) const;

	float getStart(const std::string& action) const;
	float getEnd(const std::string& action) const;
	float getLength(const std::string& action) const;

	int getIndex(const std::string& name) const;
	Matrix4 getMatrix(int bone_id, float frame) const;
	float getProperty(const std::string& name, float frame) const;

	void compileActions(float resolution);
	std::shared_ptr<Texture> getCompiledTexture(void);

	Pose armature;
	std::vector<std::string> names;
	std::map<std::string, Action> actions;

	size_t frame_count;

	std::vector<Matrix4> compiled_actions;
	std::shared_ptr<Texture> compiled_texture;

	std::map<std::string, std::vector<float>> compiled_props;
};

#endif
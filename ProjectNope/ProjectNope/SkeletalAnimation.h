#include "Vec3.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include <vector>
#include <map>
#include <string>

#ifndef SKELETAL_ANIMATION_H
#define SKELETAL_ANIMATION_H

class Bone;
class SkeletalAnimation;

class Action
{
public:
	Action(void);
	Action(float length);
	~Action(void);

	std::vector<std::vector<std::map<float, float>>> keys;
	float length;

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

	Matrix4 transform;
	Matrix4 total_transform;
	Matrix4 total_inverse;
	Bone* parent;
};

#include "Resource.h"

class SkeletalAnimation
	: public Resource
{
public:
	SkeletalAnimation(instream& is);
	~SkeletalAnimation(void);

	std::shared_ptr<Pose> getPose(float time, const Action& action)const;
	std::shared_ptr<Pose> getPose(float time, const std::string& action)const;

	Pose armature;
	std::map<std::string, Action> actions;
};

#endif
#include "SkeletalAnimation.h"

#ifndef ANIMATION_POSE_LAYER_H
#define ANIMATION_POSE_LAYER_H

class AnimationPoseLayer
{
public:
	AnimationPoseLayer(void){}
	virtual ~AnimationPoseLayer(void){}

	virtual bool tick(float dTime, Pose * pose) = 0;
};

#endif
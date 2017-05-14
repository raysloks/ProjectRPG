#ifndef BLEND_UTILITY_H
#define BLEND_UTILITY_H

class Vec3;
class Quaternion;

float bu_blend(float current, float target, float a, float b, float dTime);
Vec3 bu_blend(const Vec3& current, const Vec3& target, float a, float b, float dTime);
float bu_angle(float current, float target, float a, float b, float dTime);
float bu_angle(float current, float target, float a);
Vec3 bu_sphere(Vec3 current, Vec3 target, Vec3 pref, float a, float b, float dTime); //todo: pref rot direction incase 180 deg
Vec3 bu_sphere(Vec3 current, Vec3 target, Vec3 pref, float a);
Quaternion bu_slerp(Quaternion current, Quaternion target, float a);

#endif
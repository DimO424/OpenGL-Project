#ifndef _QUATERNIONS_H_
#define _QUATERNIONS_H_

#include "Application.h"
#include "FlyCamera.h"

struct KeyFrame
{
	vec3 position;
	glm::quat rotation;
};

class Quaternions : public Application
{
public:
	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	FlyCamera m_Camera;

	KeyFrame m_hipFrames[2];
	KeyFrame m_kneeFrames[2];
	KeyFrame m_ankleFrames[2];

	mat4 m_hipBone;
	mat4 m_kneeBone;
	mat4 m_ankleBone;

	float m_ftimer;
};

#endif
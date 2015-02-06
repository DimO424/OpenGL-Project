#ifndef _FLY_CAMERA_H_
#define _FLY_CAMERA_H_

#include "Camera.h"

class FlyCamera : public Camera
{
public:
	FlyCamera (float a_fAspect, float a_fSpeed);

	virtual void Update(float a_fDeltaTime);

	void SetSpeed (float a_fSpeed);

	float m_fSpeed;

	vec3 m_vUp;
};

#endif
#ifndef _FLY_CAMERA_H_
#define _FLY_CAMERA_H_

#include "Camera.h"

class FlyCamera : public Camera
{
public:
	FlyCamera();
	FlyCamera (float a_fAspect, float a_fSpeed);
	virtual bool Update(float a_fDeltaTime);

	float m_fSpeed;
	float m_fSensitivity;
};

#endif
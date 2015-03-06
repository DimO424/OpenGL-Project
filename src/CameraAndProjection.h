#ifndef _CAMERA_AND_PROJECTION_H_
#define _CAMERA_AND_PROJECTION_H_

#include "Application.h"
#include "FlyCamera.h"

class CameraAndProjection : public Application
{
public:
	virtual bool Startup();
	virtual void Shutdown();
	virtual bool Update();
	virtual void Draw();

	FlyCamera m_Camera;
};

#endif
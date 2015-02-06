#ifndef _CAMERA_AND_PROJECTION_H_
#define _CAMERA_AND_PROJECTION_H_

#include "Application.h"

class CameraAndProjection : public Application
{
public:
	CameraAndProjection();
	virtual ~CameraAndProjection();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();
};

#endif
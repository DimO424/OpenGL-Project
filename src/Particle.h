#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "Application.h"
#include "FlyCamera.h"
#include "Gizmos.h"
#include "GLMHeader.h"
#include "Emitter.h"
#include "Utility.h"

class Particles : public Application
{
public:
	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	FlyCamera m_Camera;
	Emitter m_Emitter;

	unsigned int m_programID;
};

#endif
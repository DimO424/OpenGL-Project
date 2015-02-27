#ifndef _GPUPARTICLE_H_
#define _GPUPARTICLE_H_

#include "Application.h"
#include "FlyCamera.h"

#include "GPUEmitter.h"

class GPUParticles : public Application
{
public:
	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	FlyCamera m_Camera;

	GPUPointEmitter m_emitter;

	float m_time;
};

#endif
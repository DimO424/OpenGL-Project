#ifndef _RENDER_TARGETS_H_
#define _RENDER_TARGETS_H_

#include "Application.h"
#include "FlyCamera.h"
#include "Vertex.h"

#include <stdio.h>

class RenderTargets : public Application
{
public:
	virtual bool Startup();
	virtual void Shutdown();
	virtual bool Update();
	virtual void Draw();

	void GenerateFrameBuffer();
	void GeneratePlane();

	FlyCamera m_Camera;

	unsigned int m_fbo;
	unsigned int m_fbo_texture;
	unsigned int m_fbo_depth;

	OpenGLData m_plane;

	unsigned int m_program_id;

	float m_timer;
};

#endif
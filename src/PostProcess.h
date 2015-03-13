#ifndef _POST_PROCESS_H_
#define _POST_PROCESS_H_

#include "Application.h"
#include "FlyCamera.h"
#include "Vertex.h"

class PostProcess : public Application
{
public:
	virtual bool Startup();
	virtual void Shutdown();
	virtual bool Update();
	virtual void Draw();

	void GenerateFrameBuffer();
	void GenerateScreenSpaceQuad();

	unsigned int m_fbo;
	unsigned int m_fbo_texture;
	unsigned int m_fbo_depth;

	unsigned int m_post_program_id;

	OpenGLData m_quad;

	FlyCamera m_Camera;

	float m_timer;
};

#endif
#ifndef _ADVANCED_TEXTURING_H_
#define _ADVANCED_TEXTURING_H_

#include "Application.h"
#include "FlyCamera.h"
#include "Vertex.h"

class AdvancedTexturing : public Application
{
public:
	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	void GenerateQuad(float a_size);
	void LoadTextures();

	OpenGLData m_quad;

	unsigned int m_programID;
	unsigned int m_diffues_texture;
	unsigned int m_normal_texture;

	FlyCamera m_Camera;
};

#endif
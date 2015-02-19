#ifndef _ADVANCED_TEXTURING_H_
#define _ADVANCED_TEXTURING_H_

#include "Application.h"
#include "FlyCamera.h"
#include "Vertex.h"

#include "AntTweakBar.h"

class AdvancedTexturing : public Application
{
public:
	AdvancedTexturing();
	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	void GenerateQuad(float a_size);
	void LoadTextures();

	OpenGLData m_quad;
	FlyCamera m_Camera;

	unsigned int m_programID;
	unsigned int m_diffuse_texture;
	unsigned int m_normal_texture;
	unsigned int m_specular_texture;

	vec3 m_lightDir;
	vec3 m_lightColor;
	vec3 m_ambientLight;
	float m_specularPower;
};

#endif
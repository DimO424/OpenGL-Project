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
	virtual ~AdvancedTexturing();

	virtual bool Startup();
	virtual void Shutdown();
	virtual bool Update();
	virtual void Draw();

	void LoadTextures();
	void GenerateQuad(float a_size);
	void ReloadShader();
	void GenerateCircle(float a_radius, int a_rows, int a_cols);

	OpenGLData m_quad;
	FlyCamera m_Camera;

	float m_timer;

	unsigned int m_programID;
	unsigned int m_diffuse_texture;
	unsigned int m_normal_texture;
	unsigned int m_specular_texture;

	vec3 m_lightDir;
	vec3 m_lightColor;
	vec3 m_ambientLight;
	vec3 m_materialColour;

	vec4 m_backgroundColor;

	float m_specularPower;
	float m_FPS;

	TwBar* m_bar;

	bool m_drawGizmos;
};

#endif
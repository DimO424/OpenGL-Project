#ifndef _SHADOWS_H_
#define _SHADOWS_H_

#include "Application.h"
#include "FlyCamera.h"
#include "Vertex.h"


class Shadows : public Application
{
public:
	virtual bool Startup();
	virtual void Shutdown();
	virtual bool Update();
	virtual void Draw();

	void BuildMeshes();
	void BuildShadowMap();

	OpenGLData m_bunny;
	OpenGLData m_plane;

	unsigned int m_fbo;
	unsigned int m_fbo_depth;

	unsigned int m_shadowmap_program_id;
	unsigned int m_diffuse_shadowed_program_id;

	vec3 m_light_direction;
	mat4 m_light_matrix;

	FlyCamera m_Camera;

	float m_timer;
};

#endif
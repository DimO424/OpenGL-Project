#ifndef _DEFFERED_RENDERING_H_
#define _DEFFERED_RENDERING_H_

#include "Application.h"
#include "FlyCamera.h"
#include "Vertex.h"

class DefferedRendering : public Application
{
public:
	virtual bool Startup();
	virtual void Shutdown();
	virtual bool Update();
	virtual void Draw();

	void BuildMeshes();
	void BuildQuad();
	void BuildCube();
	void BuildGBuffer();
	void BuildLightBuffer();

	void RenderDirectionalLight(vec3 light_dir, vec3 light_color);
	void RenderPointLight(vec3 position, float radius, vec3 diffuse);

	//Load up a mesh
	OpenGLData m_light_cube;
	OpenGLData m_bunny;
	OpenGLData m_screenspace_quad;

	//Generate a G-buffer
	unsigned int m_gbuffer_fbo;
	unsigned int m_albedo_texture;
	unsigned int m_position_texture;
	unsigned int m_normals_texture;
	unsigned int m_gbuffer_depth;

	//Render lights
	unsigned int m_light_fbo;
	unsigned int m_light_texture;

	//We need our shaders
	unsigned int m_gbuffer_program_id;
	unsigned int m_directional_light_program_id;
	unsigned int m_point_light_program_id;
	unsigned int m_spot_light_program_id;
	unsigned int m_composite_light_program_id;

	FlyCamera m_Camera;

	float m_timer;
};

#endif
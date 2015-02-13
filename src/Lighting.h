#ifndef _LIGHTING_H_
#define _LIGHTING_H_

#include "Application.h"

#include "FlyCamera.h"
#include "Utility.h"
#include "tiny_obj_loader.h"

struct OpenGlData
{
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_indexCount;

};

class Lighting : public Application
{
public:
	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	void CreateOpenGlBuffers(std::vector<tinyobj::shape_t>& shapes);
	void CleanupOpenGLBuffer();
	void ReloadShader();

	FlyCamera m_camera;

	unsigned int m_programID;
	std::vector<OpenGlData> m_gl_data;

	vec3 m_light_dir;
	vec3 m_light_color;
	vec3 m_material_color;
	vec3 m_ambient_light;

	float m_fSpecular_power;
};

#endif
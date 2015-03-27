#ifndef _PROCEDURAL_GENERATION_H_
#define _PROCEDURAL_GENERATION_H_

#include "Application.h"
#include "FlyCamera.h"
#include "Vertex.h"


class ProceduralGeneration : public Application
{
public:
	virtual bool Startup();
	virtual void Shutdown();
	virtual bool Update();
	virtual void Draw();

	void BuildGrid(vec2 real_dims, glm::ivec2 dims);
	void BuildPerlinTexture(glm::ivec2 dims, int octaves, float persistance);

	OpenGLData m_plane_mesh;
	unsigned int m_program_id;
	unsigned int m_perlin_texture;
	float* m_perlin_data;

	FlyCamera m_Camera;

	float m_timer;
};

#endif
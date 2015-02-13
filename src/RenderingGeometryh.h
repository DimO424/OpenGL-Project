#ifndef _RENDERING_GEOMETRY_H_
#define _RENDERING_GEOMETRY_H_

#include "Application.h"
#include "GLMHeader.h"
#include "FlyCamera.h"

class RenderingGeometry : public Application
{
public:
	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	void GenerateGrid(unsigned int rows, unsigned int cols);

	void GenerateShader();
	unsigned int m_programID;
	unsigned int m_index_count;

	FlyCamera m_camera;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
};

#endif
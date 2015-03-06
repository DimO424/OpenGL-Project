#ifndef _RENDERING_GEOMETRY_H_
#define _RENDERING_GEOMETRY_H_

#include "Application.h"
#include "GLMHeader.h"
#include "FlyCamera.h"

class RenderingGeometry : public Application
{
public:
	RenderingGeometry();
	~RenderingGeometry();

	virtual bool Startup();
	virtual void Shutdown();
	virtual bool Update();
	virtual void Draw();

	void GenerateGrid(unsigned int a_rows, unsigned int a_cols);
	void GenerateShader();

	unsigned int m_programID;
	unsigned int m_index_count;

	FlyCamera m_camera;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_rows;
	unsigned int m_cols;
	float m_timer;

};

#endif
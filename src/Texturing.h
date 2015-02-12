#ifndef _TEXTURING_H_
#define _TEXTURING_H_

#include "Application.h"
#include "FlyCamera.h"

class Texturing : public Application
{
public:
	Texturing();
	virtual ~Texturing();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	void LoadTexture(const char* a_filename);
	void GenerateQuad(float a_size);

	GLFWwindow* m_pWindow;
	FlyCamera m_camera;

	unsigned int m_texture;
	unsigned int m_program_id;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

private:
};

#endif
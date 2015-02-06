#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

struct GLFWwindow;

class Application
{
public:
	Application();
	virtual ~Application();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	GLFWwindow* m_pWindow;

private:


};

#endif
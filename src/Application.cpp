#include "Application.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include <cstdio>

Application::Application()
{

}

Application::~Application()
{

}

bool Application::Startup()
{
	if (glfwInit() == false)
	{
		return false;
	}

	this -> m_pWindow = glfwCreateWindow(1280, 720, "Computer Graphics", nullptr, nullptr);

	if (this -> m_pWindow == nullptr)
	{
		return false;
	}

	glfwMakeContextCurrent(this -> m_pWindow);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(this -> m_pWindow);
		glfwTerminate();
		return false;
	}

	int major_version = ogl_GetMajorVersion();
	int minor_version = ogl_GetMinorVersion();

	printf("Successfully loaded OpenGL version %d.%d \n",
		major_version, minor_version);

	return true;
}

void Application::Shutdown()
{
	glfwDestroyWindow(this->m_pWindow);
	glfwTerminate();
}

bool Application::Update()
{
	if (glfwWindowShouldClose(this->m_pWindow) == true)
	{
		return false;
	}

	return true;
}

void Application::Draw()
{

}
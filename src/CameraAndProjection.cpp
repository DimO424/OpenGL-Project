#include "CameraAndProjection.h"

CameraAndProjection::CameraAndProjection()
{

}

CameraAndProjection:: ~CameraAndProjection()
{

}

bool CameraAndProjection::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	return true;
}

void CameraAndProjection::Shutdown()
{
	Application::Shutdown();

	Gizmos::destroy();
}

bool CameraAndProjection::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0f);

	// Gizmos::draw(m_projection, m_view);
	return Application::Update();

}

void CameraAndProjection::Draw()
{
	// Gizmos::draw(m_projection, m_view);

	glfwSwapBuffers(this->m_pWindow);
	glfwPollEvents();
}
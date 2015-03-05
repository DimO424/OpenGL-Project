#include "GPUParticle.h"

bool GPUParticles::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();
	glfwSetTime(0.0f);

	m_Camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	m_Camera.SetLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_Camera.m_fSensitivity = 3;

	m_emitter.Init(10000,
		vec4(0, 1, 0, 1),
		150.0f,
		0.5f,
		4.0f,
		0.7f,
		2.0f,
		1.0f,
		0.01f,
		vec4(0.2f, 0.4f, 1.0f, 1),
		vec4(1, 0.4f, 0, 1));

	m_time = 0;
	return true;
}

void GPUParticles::Shutdown()
{
	//Now clean up
	Gizmos::destroy();
	Application::Shutdown();
}

bool GPUParticles::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0f);

	vec4 white(1);
	vec4 black(0, 0, 0, 1);

	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0.01f, -10), vec3(-10 + i, -0.01f, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, -0.01f, -10 + i), vec3(10, -0.01f, -10 + i),
			i == 10 ? white : black);
	}

	m_Camera.Update(dt);

	return true;
}

void GPUParticles::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::draw(m_Camera.projectionTransform, m_Camera.viewTransform);

	m_emitter.Draw(m_time, m_Camera.worldTransform, m_Camera.view_Projection);

	glfwSwapBuffers(this->m_pWindow);
	glfwPollEvents();
}
#include "Particle.h"

bool Particles::Startup()
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

	LoadShader("./Shaders/ParticleVertex.glsl", "./Shaders/ParticleFragment.glsl", &m_programID);

	m_Emitter.Init(10000, vec4(0, 0, 0, 1), 4000, 0.1f, 0.5f, 1, 20, 0.01f, 0.05f, vec4(1, 0.4f, 0, 1),
						vec4(1, 1, 0.8f, 1));

	return true;
}

void Particles::Shutdown()
{
	Gizmos::destroy();
	Application::Shutdown();
}

bool Particles::Update()
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

	m_Emitter.Update(dt);
	m_Emitter.UpdateVertexData(m_Camera.worldTransform[3].xyz, m_Camera.worldTransform[1].xyz);

	return true;
}

void Particles::Draw()
{
	glUseProgram(m_programID);

	int projection_view_uniform = glGetUniformLocation(m_programID, "projection_view");

	glUniformMatrix4fv(projection_view_uniform, 1, GL_FALSE, (float*)&(m_Camera.view_Projection));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_Camera.projectionTransform, m_Camera.viewTransform);
	m_Emitter.Render();

	glfwSwapBuffers(this->m_pWindow);
	glfwPollEvents();
}
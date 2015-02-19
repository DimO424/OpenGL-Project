#include "Quaternions.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

bool Quaternions::Startup()
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

	float PI = 3.14159;

	m_hipFrames[0].position = vec3(0, 5, 0);
	m_hipFrames[0].rotation = glm::quat(vec3(1, 0, 0));

	m_kneeFrames[0].position = vec3(0, -2.5f, 0);
	m_kneeFrames[0].rotation = glm::quat(vec3(1, 0, 0));

	m_ankleFrames[0].position = vec3(0, -2.5f, 0);
	m_ankleFrames[0].rotation = glm::quat(vec3(-1, 0, 0));

	return true;
}

mat4 EvaluateKeyFrames(KeyFrame a_start, KeyFrame a_end, float a_t)
{
	vec3 pos = glm::mix(a_start.position, a_end.position, a_t);
	glm::quat rot = glm::slerp(a_start.rotation, a_end.rotation, a_t);
	mat4 result = glm::translate(pos) * glm::toMat4(rot);

	return result;
}

void Quaternions::Shutdown()
{
	Gizmos::destroy();
	Application::Shutdown();
}

bool Quaternions::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	Gizmos::clear();

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0f);

	m_ftimer += dt;
	float sin_wave = sinf(m_ftimer) * 0.5f + 0.5f;

	m_hipBone = EvaluateKeyFrames(m_hipFrames[0], m_hipFrames[1], 0);
	m_kneeBone = EvaluateKeyFrames(m_kneeFrames[0], m_kneeFrames[1], 0);
	m_ankleBone = EvaluateKeyFrames(m_ankleFrames[0], m_ankleFrames[1], 0);

	mat4 global_hip = m_hipBone;
	mat4 global_knee = m_hipBone * m_kneeBone;
	mat4 global_ankle = global_knee * m_ankleBone;

	vec3 hip_pos = global_hip[3].xyz;
	vec3 knee_pos = global_knee[3].xyz;
	vec3 ankle_pos = global_ankle[3].xyz;

	Gizmos::addAABBFilled(hip_pos, vec3(0.5f), vec4(1, 1, 0, 1), &global_hip);
	Gizmos::addAABBFilled(knee_pos, vec3(0.5f), vec4(1, 0, 1, 1), &global_knee);
	Gizmos::addAABBFilled(ankle_pos, vec3(0.5f), vec4(0, 1, 1, 1), &global_ankle);

	Gizmos::addLine(hip_pos, knee_pos, vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));
	Gizmos::addLine(knee_pos, ankle_pos, vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));


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

void Quaternions::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::draw(m_Camera.projectionTransform, m_Camera.viewTransform);

	glfwSwapBuffers(this->m_pWindow);
	glfwPollEvents();
}
#include "FlyCamera.h"

FlyCamera::FlyCamera()
{

}

FlyCamera::FlyCamera(float a_fAspect, float a_fNewSpeed) : Camera(a_fAspect)
{
	this -> m_fSpeed = a_fNewSpeed;
}

bool FlyCamera::Update(float a_fDeltaTime)
{
	GLFWwindow* currentWindow = glfwGetCurrentContext();

	if (glfwGetKey(currentWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		worldTransform[3] -= worldTransform[2] * m_fSpeed * a_fDeltaTime;
	}

	if (glfwGetKey(currentWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		worldTransform[3] += worldTransform[2] * m_fSpeed * a_fDeltaTime;
	}

	if (glfwGetKey(currentWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		worldTransform[3] -= worldTransform[0] * m_fSpeed * a_fDeltaTime;
	}

	if (glfwGetKey(currentWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		worldTransform[3] += worldTransform[0] * m_fSpeed * a_fDeltaTime;
	}

	if (glfwGetKey(currentWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		worldTransform[3] -= worldTransform[1] * m_fSpeed * a_fDeltaTime;
	}

	if (glfwGetKey(currentWindow, GLFW_KEY_E) == GLFW_PRESS)
	{
		worldTransform[3] += worldTransform[1] * m_fSpeed * a_fDeltaTime;
	}

	//Now do the mouse stuff
	if (glfwGetMouseButton(currentWindow, 1))
	{
		//Do the mouse stuff if the right button has been pressed
		double x_delta;
		double y_delta;

		glfwGetCursorPos(currentWindow, &x_delta, &y_delta);
		glfwSetCursorPos(currentWindow, 1280.f / 2.f, 720.f / 2.f);

		//Calculate x and y deltas here ...
		x_delta -= (1280.f / 2.f);
		y_delta -= (720.f / 2.f);

		x_delta /= (-1280.f / 2.f);
		y_delta /= (-720.f / 2.f);

		vec3 camera_right = (vec3)worldTransform[0];

		mat4 yaw = glm::rotate((float)x_delta, vec3(0, 1, 0));
		mat4 pitch = glm::rotate((float)y_delta, camera_right);
		mat4 rotation = yaw * pitch;

		worldTransform[0] = rotation * worldTransform[0];
		worldTransform[1] = rotation * worldTransform[1];
		worldTransform[2] = rotation * worldTransform[2];

		x_delta *= -m_fSensitivity;
		y_delta *= -m_fSensitivity;
	}

	worldTransform[3][3] = 1;
	viewTransform = glm::inverse(worldTransform);
	
	UpdateViewProjection();

	return true;
}

#include "Camera.h"
#include "GLFW/glfw3.h"

Camera::Camera()
{

}

Camera::Camera(float aspect)
{
	viewTransform = glm::lookAt(vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 1, 0));
	worldTransform = glm::inverse(viewTransform);
	projectionTransform = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 1000.0f);
	view_Projection = projectionTransform * viewTransform;
}

void Camera::UpdateViewProjection()
{
	view_Projection = projectionTransform * viewTransform;
}

void Camera::SetPosition(vec3 a_vPos)
{
	worldTransform = glm::translate(a_vPos);
	viewTransform = glm::inverse(worldTransform);
	UpdateViewProjection();
}

void Camera::SetPerspective(float a_fFieldOfView, float a_fAspectRatio, float a_fnear, float a_fFar)
{
	projectionTransform = glm::perspective(a_fFieldOfView, a_fAspectRatio, a_fnear, a_fFar);
}

void Camera::SetLookAt(vec3 a_vPosition, vec3 a_vCenter, vec3 a_vUp)
{
	viewTransform = glm::lookAt(a_vPosition, a_vCenter, a_vUp);
	worldTransform = glm::inverse(viewTransform);
	UpdateViewProjection();
}
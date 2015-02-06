#include "Camera.h"

Camera:: Camera()
{

}

Camera:: Camera(vec3 pos, vec3 lookAt, float near, float far, float aspect)
{
	viewTransform = glm::lookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	projectionTransform = glm::perspective(glm::radians(55.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
}

void Camera::UpdateViewProjection()
{
	view_Projection = projectionTransform * viewTransform;
}

void Camera:: SetPerspective(float a_fFieldOfView, float a_fAspectRatio, float a_fnear, float a_fFar)
{

}

void Camera::SetLookAt(vec3 a_vFrom, vec3 a_vTo, vec3 a_vUp)
{

}
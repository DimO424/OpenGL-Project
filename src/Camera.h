#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "GLFW/glfw3.h"

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

class Camera
{
public:
	Camera();
	Camera(float aspect);

	void UpdateViewProjection();
	void SetPosition(vec3 a_vPos);
	
	void SetPerspective(float a_fFieldOfView, float a_fAspectRatio, float a_fnear, float a_fFar);
	void SetLookAt(vec3 a_vPosition, vec3 a_vCenter, vec3 a_vUp);

	virtual void Update(float a_fDeltaTime) = 0;

	mat4 worldTransform;
	mat4 viewTransform;
	mat4 projectionTransform;
	mat4 view_Projection;

};

#endif
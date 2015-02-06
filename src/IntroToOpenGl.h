#ifndef _INTRO_TO_OPENGL_H_
#define _INTRO_TO_OPENGL_H_

#include "Application.h"
#include "Gizmos.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

class IntroToOpenGl : public Application
{
public:
	IntroToOpenGl();
	virtual ~IntroToOpenGl();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	mat4 m_view;
	mat4 m_projection;

	float m_cameraX;
	float m_cameraZ;
	float m_timer;
};


#endif
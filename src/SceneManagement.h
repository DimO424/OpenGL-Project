#ifndef _SCENE_MANAGEMENT_H_
#define _SCENE_MANAGEMENT_H_

#include "Application.h"
#include "FlyCamera.h"

#include "AntTweakBar.h"

class SceneManagement : public Application
{
public:
	SceneManagement();
	virtual ~SceneManagement();

	virtual	bool Startup();
	virtual	void Shutdown();
	virtual	bool Update();
	virtual	void Draw();

	void getFrustumPlanes(const glm::mat4& a_transform, glm::vec4* a_planes);

	float m_timer;

	FlyCamera m_camera;

	vec4 m_backgroundColor;

	float m_FPS;

	TwBar* m_bar;

	bool m_drawGizmos;
};

#endif
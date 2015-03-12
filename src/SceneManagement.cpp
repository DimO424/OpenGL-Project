#include "SceneManagement.h"
#include "GLMHeader.h"
#include "Gizmos.h"
#include "Utility.h"
#include "AABB.h"
#include "BoundingSphere.h"
#include "AdvancedTexturing.h"

SceneManagement::SceneManagement()
{

}

SceneManagement::~SceneManagement()
{

}

bool SceneManagement::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();
	glfwSetTime(0.0f);

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);
	//	setup callbacks to send info to AntTweakBar
	glfwSetMouseButtonCallback(m_pWindow, OnMouseButton);
	glfwSetCursorPosCallback(m_pWindow, OnMousePosition);
	glfwSetScrollCallback(m_pWindow, OnMouseScroll);
	glfwSetKeyCallback(m_pWindow, OnKey);
	glfwSetCharCallback(m_pWindow, OnChar);
	glfwSetWindowSizeCallback(m_pWindow, OnWindowResize);

	m_backgroundColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
	
	m_drawGizmos = true;

	//Now initialise the FlyCamera
	m_camera = FlyCamera(1280.f/720.f, 10.0f);
	m_camera.SetLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_camera.m_fSensitivity = 3;

	//Initialise basic AntTweakBar info
	m_bar = TwNewBar("Amazing new AntTweakBar!!");
	TwAddSeparator(m_bar, "Light Data", "");
	//TwAddVarRW(m_bar, "Light Direction", TW_TYPE_DIR3F, &m_light_dir, "label='Group Light'");
	//TwAddVarRW(m_bar, "Light Colour", TW_TYPE_COLOR3F, &m_light_colour, "label='Group Light'");
	//TwAddVarRW(m_bar, "Spec Power", TW_TYPE_FLOAT, &m_fSpecular_power, "label='Group Light' min=0.05 max=100 step=0.05");
	TwAddSeparator(m_bar, "Misc Data", "");
	TwAddVarRW(m_bar, "Clear Colour", TW_TYPE_COLOR4F, &m_backgroundColor, "");
	TwAddVarRW(m_bar, "Draw Gizmos", TW_TYPE_BOOL8, &m_drawGizmos, "");
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_FPS, "");
	
	return true;
}

void SceneManagement::Shutdown()
{
	//Now clean up
	Gizmos::destroy();
	TwDeleteAllBars();
	TwTerminate();
}

bool SceneManagement::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	//Check if we need to reload the shaders
	if (glfwGetKey(m_pWindow, GLFW_KEY_R) == GLFW_PRESS)
	{
		//ReloadShader();
	}
	
	float dT = (float)glfwGetTime();
	glfwSetTime(0.0f);
	m_FPS = (float)(1.0 / dT);

	//Now we get to the fun stuff
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::clear();
	Gizmos::addTransform(mat4(1));
	
	m_timer += dT;
	m_camera.Update(dT);
	
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	vec4 blue(0, 0, 1, 1);
	vec4 yellow(1, 1, 0, 1);
	vec4 green(0, 1, 0, 1);
	vec4 red(1, 0, 0, 1);

	for (unsigned int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
				i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
				i == 10 ? white : black);
	}
	
	//A plane that faces up
	//Offset from (0,0,0) by 1
	vec4 plane(0, 1, 0, -1);
	// ax + by + cz + d = 0
	// 0 + 1 + 0 + -1 = 0
	
	BoundingSphere sphere;
	sphere.centre = vec3(0, 2 * cosf(m_timer * 5) + 1, 0);
	sphere.radius = 0.5f;
	
	float d = glm::dot(vec3(plane), sphere.centre) + plane.w;
	
	vec4 planeColour(1, 1, 0, 1);
	
	if (d > sphere.radius)
	{
		printf ("Front\n");
		planeColour = vec4(0, 1, 0, 1);
	}
	else if (d < -sphere.radius)
	{
		planeColour = vec4(1, 0, 0, 1);
		printf("Back\n");
	}
	else
	{
		// printf("On the plane\n");
	}
		Gizmos::addTri(vec3(4, 1, 4), vec3(-4, 1, -4), vec3(-4, 1, 4), planeColour);
		Gizmos::addTri(vec3(4, 1, 4), vec3(4, 1, -4), vec3(-4, 1, -4), planeColour);
	
	vec4 planes[6];

	getFrustumPlanes((m_camera.projectionTransform, m_camera.viewTransform), planes);

	bool sphereVisible = true;

	for (unsigned int i = 0; i < 6; i++)
	{
		float d = glm::dot(vec3(planes[i]), sphere.centre) + planes[i].w;

		if (d < -sphere.radius)
		{
			sphereVisible = false;
			printf("Behind, don't render it!\n");
			break;
		}
		else if (d < sphere.radius)
		{
			printf("Touching, we still need to render it!\n");
		}
		else
		{
			printf("Front, fully visible so render it!\n");
		}
	}

	if (sphereVisible)
	{
		Gizmos::addSphere(sphere.centre, sphere.radius, 8, 8, vec4(1, 0, 1, 1));
	}
	
	glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, m_backgroundColor.w);
	return true;
}

void SceneManagement::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_drawGizmos)
	{
		Gizmos::draw(m_camera.projectionTransform, m_camera.viewTransform);
	}
	
	TwDraw();
	Application::Draw();

	glfwSwapBuffers(m_pWindow);
	glfwPollEvents();
}

void SceneManagement::getFrustumPlanes(const glm::mat4& a_transform, glm::vec4* a_planes)
{
	//Right side
	a_planes[0] = vec4(a_transform[0][3] - a_transform[1][0],
		a_transform[1][3] - a_transform[1][0],
		a_transform[2][3] - a_transform[2][0],
		a_transform[3][3] - a_transform[3][0]);

	//Left side
	a_planes[1] = vec4(a_transform[0][3] + a_transform[0][0],
		a_transform[1][3] + a_transform[1][0],
		a_transform[2][3] + a_transform[2][0],
		a_transform[3][3] + a_transform[3][0]);

	//Top
	a_planes[2] = vec4(a_transform[0][3] - a_transform[0][1],
		a_transform[1][3] - a_transform[1][1],
		a_transform[2][3] - a_transform[2][1],
		a_transform[3][3] - a_transform[3][1]);

	//Bottom
	a_planes[3] = vec4(a_transform[0][3] + a_transform[0][1],
		a_transform[1][3] + a_transform[1][1],
		a_transform[2][3] + a_transform[2][1],
		a_transform[3][3] + a_transform[3][1]);

	//Far
	a_planes[4] = vec4(a_transform[0][3] - a_transform[0][2],
		a_transform[1][3] - a_transform[1][2],
		a_transform[2][3] - a_transform[2][2],
		a_transform[3][3] - a_transform[3][2]);

	//Near
	a_planes[5] = vec4(a_transform[0][3] + a_transform[0][2],
		a_transform[1][3] + a_transform[1][2],
		a_transform[2][3] + a_transform[2][2],
		a_transform[3][3] + a_transform[3][2]);

	for (unsigned int i = 0; i < 6; i++)
	{
		a_planes[i] = glm::normalize(a_planes[i]);
	}
}
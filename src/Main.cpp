#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include <cstdio>

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

int main()
{
	if (glfwInit() == false)
	{
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(1280, 720,
									"Computer Graphics", nullptr, nullptr);

	if (window == nullptr)
	{
		return -2;
	}

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	int major_version = ogl_GetMajorVersion();
	int minor_version = ogl_GetMinorVersion();

	printf("Successfully loaded OpenGL version %d.%d \n",
												major_version, minor_version);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	mat4 view = glm::lookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 projection = glm::perspective(glm::radians(55.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

	float cameraX = -6.0f;
	float cameraZ = -6.0f;

	float timer = 0.0f;

	glfwSetTime(0.0);
	while (glfwWindowShouldClose(window) == false)
	{
		float dt = (float)glfwGetTime();
		glfwSetTime(0.0);

		timer += dt;
		// cameraX = sinf(timer) * 10;
		// cameraZ = cosf(timer) * 10;

		mat4 view = glm::lookAt(vec3(cameraX, 10, cameraZ), vec3(0, 0, 0), vec3(0, 1, 0));
		// cameraX += dt;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Gizmos::clear();

		Gizmos::addTransform( mat4(1) );

		vec4 white (1);
		vec4 black (0, 0, 0 ,1);
		vec4 red (1, 0, 0, 1);
		vec4 green (0, 1, 0, 1);
		vec4 blue (0, 0, 1, 1);
		vec4 yellow(247, 255, 0, 1);

		for (int i = 0; i <= 20; ++i)
		{
			Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
														i == 10 ? white : black);
			Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
														i == 10 ? white : black);
		}

		mat4 rotation_matrix = glm::rotate(timer, vec3(0, 1, 0)); // rotation matrix

		mat4 oPlanet_one_transform = rotation_matrix * glm::translate(vec3(3, 0, 0)) * glm::rotate(timer * 3, vec3(0, 3, 0));
		mat4 oPlanet_two_transform = rotation_matrix * glm::translate(vec3(4.2f, 0, 0)) * glm::rotate(timer * 5, vec3(0, 2, 0));
		mat4 oPlanet_three_transform = (rotation_matrix * glm::translate(vec3(6.2f, 0, 0)) * glm::rotate(timer, vec3(0, 1, 0));

		mat4 oMoon_one_transform = oPlanet_one_transform * glm::translate(vec3(1.5f, 0, 0));

		// mat4 translation_matrix = glm::translate(vec3(5, 0, 0)); // translation matrix
		// mat4 transform = rotation_matrix * translation_matrix;

		Gizmos::addSphere(rotation_matrix[3].xyz, 2.0f, 25, 25, yellow, &rotation_matrix);

		Gizmos::addSphere(oPlanet_one_transform[3].xyz, 0.5f, 20, 20, blue, &oPlanet_one_transform);
		Gizmos::addSphere(oPlanet_two_transform[3].xyz, 0.8f, 20, 20, blue, &oPlanet_two_transform);
		Gizmos::addSphere(oPlanet_three_transform[3].xyz, 1.3f, 20, 20, blue, &oPlanet_three_transform);

		Gizmos::addSphere(oMoon_one_transform[3].xyz, 0.1f, 20, 20, red, &oMoon_one_transform);

		// Gizmos::addTri(vec3(0, 1, 0), vec3(2, -1, 1), vec3(-3, -2, 4), green);

		// Gizmos::addAABB(vec3(2.5f, 5, 0), vec3(1, 1, 1), blue);

		// Gizmos::addAABBFilled(vec3(-2.5f, 5, 0), vec3(1, 1, 1), blue);

		Gizmos::draw(projection, view);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Gizmos::destroy();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
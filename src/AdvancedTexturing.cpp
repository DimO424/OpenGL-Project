#include "AdvancedTexturing.h"
#include "GLMHeader.h"
#include "Gizmos.h"
#include "Utility.h"

#define STB_IMAGE_IMPLEMENTAION
#include "stb_image.h"

void OnMouseButton(GLFWwindow*, int a_button, int a_pressed, int a_mod_keys)
{
	TwEventMouseButtonGLFW(a_button, a_pressed);
}

void OnMousePosition(GLFWwindow*, double a_x, double a_y)
{
	TwEventMousePosGLFW((int)a_x, (int)a_y);
}

void OnMouseScroll(GLFWwindow*, double a_x, double a_y)
{
	TwEventMouseWheelGLFW((int)a_y);
}

void OnKey(GLFWwindow*, int a_key, int a_scan_code, int a_pressed, int a_mod_keys)
{
	TwEventKeyGLFW(a_key, a_pressed);
}

void OnChar(GLFWwindow*, unsigned int a_c)
{
	TwEventCharGLFW(a_c, GLFW_PRESS);
}

void OnWindowResize(GLFWwindow*, int a_width, int a_height)
{
	TwWindowSize(a_width, a_height);
	glViewport(0, 0, a_width, a_height);
}

AdvancedTexturing::AdvancedTexturing()
{

}

bool AdvancedTexturing::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);

	m_backColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);

	glfwSetMouseButtonCallback(m_pWindow, OnMouseButton);
	glfwSetCursorPosCallback(m_pWindow, OnMousePosition);
	glfwSetScrollCallback(m_pWindow, OnMouseScroll);
	glfwSetKeyCallback(m_pWindow, OnKey);
	glfwSetCharCallback(m_pWindow, OnChar);
	glfwSetWindowSizeCallback(m_pWindow, OnWindowResize);

	m_bar = TwNewBar("my awesome bar");
	
	TwAddVarRW("Clear Color", TW_TYPE_COLOR4F, &m_backColor, "");

	Gizmos::create();
	glfwSetTime(0.0f);

	LoadTextures();
	LoadShader("./shaders/normal_mapped_vertex.glsl", "./shaders/normal_mapped_fragment.glsl", &m_programID);
	GenerateQuad(5.0f);

	m_Camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	m_Camera.SetLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_Camera.m_fSensitivity = 3;

	m_ambientLight = vec3(0.1f);
	m_lightDir = (vec3(-1, -1, 0));
	m_lightColor = vec3(0.7f);
	m_specularPower = 15;

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	return true;
}

void AdvancedTexturing::Shutdown()
{
	Gizmos::destroy();
	Application::Shutdown();

	TwDeleteAllBars();
	TwTerminate();
}

bool AdvancedTexturing::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0f);

	Gizmos::addTransform(mat4(1));

	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	vec4 red(1, 0, 0, 1);
	vec4 green(0, 1, 0, 1);
	vec4 blue(0, 0, 1, 1);

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

void AdvancedTexturing::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::draw(m_Camera.projectionTransform, m_Camera.viewTransform);

	TwDraw();

	glfwSwapBuffers(this->m_pWindow);
	glfwPollEvents();
}

void AdvancedTexturing::GenerateQuad(float a_size)
{
	VertexNormal vertexData[4];

	vertexData[0].position = vec4(-a_size, 0, -a_size, 1);
	vertexData[1].position = vec4(-a_size, 0, a_size, 1);
	vertexData[2].position = vec4(a_size, 0, a_size, 1);
	vertexData[3].position = vec4(a_size, 0, -a_size, 1);

	vertexData[0].normal = vec4(0, 1, 0, 0);
	vertexData[1].normal = vec4(0, 1, 0, 0);
	vertexData[2].normal = vec4(0, 1, 0, 0);
	vertexData[3].normal = vec4(0, 1, 0, 0);

	vertexData[0].tangent = vec4(1, 0, 0, 0);
	vertexData[1].tangent = vec4(1, 0, 0, 0);
	vertexData[2].tangent = vec4(1, 0, 0, 0);
	vertexData[3].tangent = vec4(1, 0, 0, 0);

	vertexData[0].texCoord = vec2(0, 0);
	vertexData[1].texCoord = vec2(0, 1);
	vertexData[2].texCoord = vec2(1, 1);
	vertexData[3].texCoord = vec2(1, 0);

	unsigned int indexData[6] = { 0, 2, 1,
								0, 3, 2 };

	m_quad.m_indexCount = 6;
	glGenVertexArrays(1, &m_quad.m_VAO);

	glGenBuffers(1, &m_quad.m_VBO);
	glGenBuffers(1, &m_quad.m_IBO);

	glBindVertexArray(m_quad.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_quad.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexNormal)* 4, vertexData, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quad.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //normal
	glEnableVertexAttribArray(2); //tangent
	glEnableVertexAttribArray(3); //tex coord

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), 0);	//position
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(VertexNormal), (void*)(sizeof(vec4)* 1)); //normal
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(VertexNormal), (void*)(sizeof(vec4)* 2));	//tangent
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (void*)(sizeof(vec4)* 3)); //tex coord

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void AdvancedTexturing::LoadTextures()
{
	int width;
	int height;
	int channels;

	unsigned char* data;

	data = stbi_load("./textures/rock_diffuse.tga", 
				&width, &height, &channels, STBI_default);

	glGenTextures(1, &m_diffuse_texture);
	glBindTexture(GL_TEXTURE_2D, m_diffuse_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	data = stbi_load("./textures/rock_normal.tga",
		&width, &height, &channels, STBI_default);

	glGenTextures(1, &m_normal_texture);
	glBindTexture(GL_TEXTURE_2D, m_normal_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	data = stbi_load("./textures/rock_specular.tga",
		&width, &height, &channels, STBI_default);

	glGenTextures(1, &m_specular_texture);
	glBindTexture(GL_TEXTURE_2D, m_specular_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}
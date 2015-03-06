#include "AdvancedTexturing.h"
#include "GLMHeader.h"
#include "Gizmos.h"
#include "Utility.h"

#define STB_IMAGE_IMPLEMENTAION
#include "stb_image.h"

AdvancedTexturing::AdvancedTexturing()
{

}

AdvancedTexturing::~AdvancedTexturing()
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

	//Send window size events to AntTweakBar
	glfwSetMouseButtonCallback(m_pWindow, OnMouseButton);
	glfwSetCursorPosCallback(m_pWindow, OnMousePosition);
	glfwSetScrollCallback(m_pWindow, OnMouseScroll);
	glfwSetKeyCallback(m_pWindow, OnKey);
	glfwSetCharCallback(m_pWindow, OnChar);
	glfwSetWindowSizeCallback(m_pWindow, OnWindowResize);

	m_backgroundColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
	glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, m_backgroundColor.w);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);
	Gizmos::create();

	LoadShader("./Shaders/normal_mapped_vertex.glsl", nullptr,
			"./Shaders/normal_mapped_fragment.glsl", &m_programID);
	GenerateQuad(5.0f);

	//GenerateCircle(2.0f, 10, 10);	//	not working yet ...

	m_ambientLight = vec3(0.1f, 0.1f, 0.1f);
	m_lightDir = (vec3(0, -1, 0));
	m_lightColor = vec3(0.1f, 0.5f, 0.7f);
	m_materialColour = vec3(1.0f);
	m_specularPower = 15.0f;

	m_Camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	m_Camera.SetLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_Camera.m_fSensitivity = 3;
	LoadTextures();

	m_bar = TwNewBar("My awesome AntTweakBar");
	TwAddSeparator(m_bar, "Light Data", "");
	TwAddVarRW(m_bar, "Light Direction", TW_TYPE_DIR3F, &m_lightDir, "Group Light");
	TwAddVarRW(m_bar, "Light Colour", TW_TYPE_COLOR3F, &m_lightColor, "Group Light");
	TwAddVarRW(m_bar, "Spec Power", TW_TYPE_FLOAT, &m_specularPower, "Group Light min = 0.05 max = 100 step = 0.05");
	TwAddVarRW(m_bar, "Light Direction", TW_TYPE_DIR3F, &m_lightDir, "label = 'Group Light'");
	TwAddVarRW(m_bar, "Light Colour", TW_TYPE_COLOR3F, &m_lightColor, "label = 'Group Light'");
	TwAddVarRW(m_bar, "Spec Power", TW_TYPE_FLOAT, &m_specularPower, "label = 'Group Light' min = 0.05 max = 100 step = 0.05");
	TwAddSeparator(m_bar, "Misc Data", "");
	TwAddVarRW(m_bar, "Clear Colour", TW_TYPE_COLOR4F, &m_backgroundColor, "");
	TwAddVarRW(m_bar, "Draw Gizmos", TW_TYPE_BOOL8, &m_drawGizmos, "");
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_FPS, "");

	return true;
}

void AdvancedTexturing::Shutdown()
{
	Gizmos::destroy();
	Application::Shutdown();

	TwDeleteAllBars();
	TwTerminate();

	glDeleteProgram(m_programID);
}

bool AdvancedTexturing::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	//Check if we need to reload the shaders
	if (glfwGetKey(m_pWindow, GLFW_KEY_R) == GLFW_PRESS)
	{
		ReloadShader();
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0f);
	m_FPS = (float)(1.0 / dt);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::clear();
	Gizmos::addTransform(mat4(1));

	m_timer += dt;
	//m_lightDir = (glm::rotate(dt, vec3(0, 1, 0)) * vec4(m_lightDir, 0)).xyz;
	m_Camera.Update(dt);

	glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, m_backgroundColor.w);

	return true;
}

void AdvancedTexturing::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_programID);

	//Draw the grid
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	vec4 blue(0, 0, 1, 1);
	vec4 yellow(1, 1, 0, 1);
	vec4 green(0, 1, 0, 1);
	vec4 red(1, 0, 0, 1);

	for (unsigned int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, -0.01f, -10), vec3(-10 + i, -0.01f, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, -0.01f, -10 + i), vec3(10, -0.01f, -10 + i),
			i == 10 ? white : black);
	}

	int iViewProjUniform = glGetUniformLocation(m_programID, "projection_view");
	glUniformMatrix4fv(iViewProjUniform, 1, GL_FALSE, (float*)&m_Camera.projectionTransform);

	int	ambient_uniform = glGetUniformLocation(m_programID, "ambient_light");
	int	light_colour_uniform = glGetUniformLocation(m_programID, "light_colour");
	int	light_dir_uniform = glGetUniformLocation(m_programID, "light_dir");
	int	material_colour_uniform = glGetUniformLocation(m_programID, "material_colour");
	int	eye_pos_uniform = glGetUniformLocation(m_programID, "eye_pos");
	int	specular_uniform = glGetUniformLocation(m_programID, "specular_power");
	int	timer_uniform = glGetUniformLocation(m_programID, "timer");

	glUniform3fv(ambient_uniform, 1, (float*)&m_ambientLight);
	glUniform3fv(light_colour_uniform, 1, (float*)&m_lightColor);
	glUniform3fv(light_dir_uniform, 1, (float*)&m_lightDir);
	glUniform3fv(material_colour_uniform, 1, (float*)&m_materialColour);

	vec3 camera_pos = m_Camera.worldTransform[3].xyz;
	glUniform3fv(eye_pos_uniform, 1, (float*)&camera_pos);
	glUniform1f(specular_uniform, m_specularPower);
	glUniform1f(timer_uniform, m_timer);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_diffuse_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normal_texture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_specular_texture);

	int	diffuse_location = glGetUniformLocation(m_programID, "diffuse_tex");
	int	normal_location = glGetUniformLocation(m_programID, "normal_tex");
	int	specular_location = glGetUniformLocation(m_programID, "specular_tex");

	glUniform1i(diffuse_location, 0);
	glUniform1i(normal_location, 1);
	glUniform1i(specular_location, 2);

	//Un-comment the following line for wireframe ...
	//else
	//{
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);
	//}

	glBindVertexArray(m_quad.m_VAO);

	//if (glfwGetKey(m_window, GLFW_KEY_M) == GLFW_PRESS)
	//{
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//}
	//else
	//{
	//	//ReloadShader();
	//}

	glDrawElements(GL_TRIANGLES, m_quad.m_indexCount, GL_UNSIGNED_INT, 0);

	Application::Draw();

	if (m_drawGizmos)
	{
		Gizmos::draw(m_Camera.projectionTransform, m_Camera.viewTransform);
	}
	
	TwDraw();

	glfwSwapBuffers(this->m_pWindow);
	glfwPollEvents();
}

void AdvancedTexturing::GenerateQuad(float a_size)
{
	VertexNormal vertexData[4];
	a_size *= 0.5f; //Halve the size so that the quad is the actuall size passed in

	//Now generate the data
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

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), 0); //position
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(VertexNormal), (void*)(sizeof(vec4))); //normal
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

void AdvancedTexturing::ReloadShader()
{
	glDeleteProgram(m_programID);
	LoadShader("./Shaders/normal_mapped_vertex.glsl", nullptr,
			"./Shaders/normal_mapped_fragment.glsl", &m_programID);
}

void AdvancedTexturing::GenerateCircle(float a_radius, int a_rows, int a_cols)
{
	OpenGLData result = {};
	VertexTexCoord* verts = new VertexTexCoord[a_cols + 1];
	verts[0].position = vec4(0, 0, 0, 0);
	verts[0].tex_coord = vec2(0.5f, 0.5f);

	for (unsigned int i = 0; i < a_cols; ++i)
	{
		verts[i + 1].position = vec4((i / (float)a_cols ) * 2.0f * 3.141592654, 0,
									 (i / (float)a_cols) * 2.0f * 3.141592654, 1);
		verts[i + 1].tex_coord = vec2(verts[i + 1].position.x + 0.5f,
									  verts[i + 1].position.y + 0.5f);
	}

	unsigned int* indices = new unsigned int[3 * a_cols];

	for (unsigned int i = 0; i < a_cols; ++i)
	{
		indices[i * 3] = 0;
		indices[i * 3 + 1] = i + 1;
		indices[i * 3 + 2] = i + 2;
	}

	glGenVertexArrays(1, &m_quad.m_VAO);
	glGenBuffers(1, &m_quad.m_VAO);
	glGenBuffers(1, &m_quad.m_VBO);

	glBindVertexArray(m_quad.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_quad.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexNormal)* 4, verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quad.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // normal
	glEnableVertexAttribArray(2); // tangent
	glEnableVertexAttribArray(3); // texture coord

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), 0); // position
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(VertexNormal), (void*)(sizeof(vec4))); // normal
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(VertexNormal), (void*)(sizeof(vec4)* 2));	// tangent
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (void*)(sizeof(vec4)* 3)); // texture coord

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
#include "Lighting.h"

bool Lighting::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	Gizmos::create();

	LoadShader("./Shaders/LightingVertex.glsl", "./Shaders/LightingFragment.glsl", &m_programID);

	m_camera = FlyCamera(60.0f, 10.0f);
	m_camera.SetLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_camera.m_fSensitivity = 3;

	//sets clear colour (grey)
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string error = tinyobj::LoadObj(shapes, materials, "./models/stanford/bunny.obj");

	if (error.size() != 0)
	{
		return false;
	}

	CreateOpenGlBuffers(shapes);

	m_ambient_light = vec3(0.1f);
	m_light_dir = vec3(0, -1, 0);
	m_light_color = vec3(0.6f, 0, 0);
	m_material_color = vec3(1);

	m_fSpecular_power = 15;

	return true;
}

void Lighting::Shutdown()
{
	glfwDestroyWindow(this->m_pWindow);
	glfwTerminate();
}

void Lighting::Draw()
{
	//resets color values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);

	int view_projection_uniform = glGetUniformLocation(m_programID, "projection_view");

	glUniformMatrix4fv(view_projection_uniform, 1, GL_FALSE, (float*)&m_camera.view_Projection);

	int ambient_uniform = glGetUniformLocation(m_programID, "ambient_light" );
	int light_dir_uniform = glGetUniformLocation(m_programID, "light_dir");
	int light_color_uniform = glGetUniformLocation(m_programID, "light_color");
	int material_color_uniform = glGetUniformLocation(m_programID, "material_color");

	int eye_pos_uniform = glGetUniformLocation(m_programID, "eye_pos");
	int specular_power_uniform = glGetUniformLocation(m_programID, "specular_power");

	glUniform3fv(ambient_uniform, 1, (float*)&m_ambient_light);
	glUniform3fv(light_dir_uniform, 1, (float*)&m_light_dir);
	glUniform3fv(light_color_uniform, 1, (float*)&m_light_color);
	glUniform3fv(material_color_uniform, 1, (float*)&m_material_color);

	vec3 camera_pos = m_camera.worldTransform[3].xyz;
	glUniform3fv(eye_pos_uniform, 1,(float*)&camera_pos);

	glUniform1f(specular_power_uniform, specular_power_uniform);

	for (unsigned int meshIndex = 0; meshIndex < m_gl_data.size(); ++meshIndex)
	{
		glBindVertexArray(m_gl_data[meshIndex].m_VAO);
		glDrawElements(GL_TRIANGLES, m_gl_data[meshIndex].m_indexCount, GL_UNSIGNED_INT, 0);
	}
	//Gizmos::draw(m_camera.view_Projection);
	Gizmos::draw(m_camera.projectionTransform, m_camera.viewTransform);

	glfwSwapBuffers(this -> m_pWindow);
	glfwPollEvents();
}

bool Lighting::Update()
{
	//Determines delta time and rotation values
	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	//Define color values
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	vec4 yellow(1, 1, 0, 1);
	vec4 green(0, 1, 0, 1);
	vec4 orange(1.0f, 0.7f, 0.5f, 1.0f);

	//Adds Line Grid
	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0.01f, -10), vec3(-10 + i, -0.01f, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, -0.01f, -10 + i), vec3(10, -0.01f, -10 + i),
			i == 10 ? white : black);
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_R) == GLFW_PRESS)
	{
		ReloadShader();
	}

	m_camera.Update(dt);

	//determines if the application should stay open or not
	return Application::Update();
}

void Lighting::CreateOpenGlBuffers(std::vector<tinyobj::shape_t>& shapes)
{
	m_gl_data.resize(shapes.size());

	for (unsigned int shape_index = 0; shape_index < shapes.size(); ++shape_index)
	{
		std::vector<float> vertexData;

		unsigned int floatCount = shapes[shape_index].mesh.positions.size();

		floatCount += shapes[shape_index].mesh.normals.size();

		vertexData.reserve(floatCount);

		vertexData.insert(vertexData.end(),
			shapes[shape_index].mesh.positions.begin(),
			shapes[shape_index].mesh.positions.end());

		vertexData.insert(vertexData.end(),
			shapes[shape_index].mesh.normals.begin(),
			shapes[shape_index].mesh.normals.end());

		m_gl_data[shape_index].m_indexCount = shapes[shape_index].mesh.indices.size();

		glGenVertexArrays(1, &m_gl_data[shape_index].m_VAO);
		glBindVertexArray(m_gl_data[shape_index].m_VAO);

		glGenBuffers(1, &m_gl_data[shape_index].m_VBO);
		glGenBuffers(1, &m_gl_data[shape_index].m_IBO);

		glBindBuffer(GL_ARRAY_BUFFER, m_gl_data[shape_index].m_VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_data[shape_index].m_IBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*floatCount, vertexData.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[shape_index].mesh.indices.size()*sizeof(unsigned int),
			shapes[shape_index].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0,
				(void*)(sizeof(float)*shapes[shape_index].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void Lighting::CleanupOpenGLBuffer()
{
	glDeleteProgram(m_programID);
	
	for (unsigned int i = 0; i < m_gl_data.size(); ++i)
	{
		glDeleteVertexArrays(1, &m_gl_data[i].m_VAO);
		glDeleteBuffers(1, &m_gl_data[i].m_VBO);
		glDeleteBuffers(1, &m_gl_data[i].m_IBO);
	}
}

void Lighting::ReloadShader()
{
	
}
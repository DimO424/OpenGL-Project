#include "Shadows.h"
#include "tiny_obj_loader.h"
#include "Utility.h"

bool Shadows::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);

	Gizmos::create();

	m_Camera = FlyCamera(1280.0f / 720.0f, 30.0f);
	m_Camera.SetLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_Camera.m_fSensitivity = 3;

	LoadShader("./shaders/diffuse_shadowed_vertex.glsl", 0,
		"./shaders/diffuse_shadowed_fragment.glsl",
		&m_diffuse_shadowed_program_id);

	LoadShader("./shaders/shadowmap_vertex.glsl", 0,
		"./shaders/shadowmap_fragment.glsl",
		&m_shadowmap_program_id);

	BuildMeshes();
	BuildShadowMap();

	return true;
}

void Shadows::BuildShadowMap()
{
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glGenTextures(1, &m_fbo_depth);
	glBindTexture(GL_TEXTURE_2D, m_fbo_depth);
	glTexImage2D(GL_TEXTURE_2D, 0 , GL_DEPTH_COMPONENT16, 1024, 1024, 0, 
					GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_fbo_depth, 0);

	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shadows::BuildMeshes()
{
	std::vector <tinyobj::shape_t> shapes;
	std::vector <tinyobj::material_t> materials;

	tinyobj::LoadObj(shapes, materials, "./models/stanford/bunny.obj");

	m_bunny.m_indexCount = shapes[0].mesh.indices.size();

	tinyobj::mesh_t* mesh = &shapes[0].mesh;

	std::vector <float> vertex_data;
	vertex_data.reserve(mesh -> positions.size() + mesh -> normals.size());

	vertex_data.insert(vertex_data.end(),
		mesh->positions.begin(), mesh->positions.end());
	vertex_data.insert(vertex_data.end(),
		mesh->normals.begin(), mesh->normals.end());

	glGenVertexArrays(1, &m_bunny.m_VAO);
	glBindVertexArray(m_bunny.m_VAO);

	glGenBuffers(1, &m_bunny.m_VBO);
	glGenBuffers(1, &m_bunny.m_IBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_bunny.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bunny.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_data.size(), 
		vertex_data.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh -> indices.size(),
		mesh -> indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //Position
	glEnableVertexAttribArray(1); //Normal

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(float) * mesh -> positions.size()));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//PLANE
	float plane_vertex_data[] = 
	{
		-10, 0, -10, 1,			0, 1, 0, 0,
		 10, 0, -10, 1,			0, 1, 0, 0,
		 10, 0,  10, 1,			0, 1, 0, 0,
		-10, 0,  10, 1,			0, 1, 0, 0,
	};

	unsigned int plane_index_data[]
	{
		0, 1, 2, 0, 2, 3
	};

	m_plane.m_indexCount = 6;

	glGenVertexArrays(1, &m_plane.m_VAO);
	glBindVertexArray(m_plane.m_VAO);
	glGenBuffers(1, &m_plane.m_VBO);
	glGenBuffers(1, &m_plane.m_IBO);


	glBindBuffer(GL_ARRAY_BUFFER, m_plane.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertex_data), plane_vertex_data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_index_data), plane_index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Shadows::Shutdown()
{
	Gizmos::destroy();
	Application::Shutdown();
}

bool Shadows::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	float dT = (float)glfwGetTime();
	glfwSetTime(0.0f);

	Gizmos::clear();
	Gizmos::addTransform(mat4(1));

	m_timer += dT;

	m_Camera.Update(dT);

	vec4 black(0, 0, 0, 1);
	vec4 blue(0, 0, 1, 1);
	vec4 yellow(1, 1, 0, 1);
	vec4 green(0, 1, 0, 1);
	vec4 red(1, 0, 0, 1);
	vec4 white(1);

	for (unsigned int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
			i == 10 ? white : black);
	}

	return true;
}

void Shadows::Draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, 1024, 1024);

	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_shadowmap_program_id);

	int light_matrix_location =
		glGetUniformLocation(m_shadowmap_program_id, "light_matrix");

	vec3 light_dir = glm::normalize(vec3(-1, -2.5f, -1));
	// light_dir = (glm::rotate(0.01f, ));

	mat4 light_proj = glm::ortho<float>(-15.0f, 15.0f, -15.0f, 15.0f, -10, 100);

	mat4 light_view = glm::lookAt(-light_dir, vec3(0), vec3(0, 1, 0));

	mat4 light_matrix = light_proj * light_view;

	glUniformMatrix4fv(light_matrix_location, 1,
							GL_FALSE, (float*)&light_matrix);

	glBindVertexArray(m_bunny.m_VAO);
	glDrawElements(GL_TRIANGLES, m_bunny.m_indexCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(m_plane.m_VAO);
	glDrawElements(GL_TRIANGLES, m_plane.m_indexCount, GL_UNSIGNED_INT, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_diffuse_shadowed_program_id);

	mat4 offset_scale = mat4(
					0.5,  0,    0,    0,
					0,    0.5f, 0,    0, 
					0,    0,    0.5f, 0, 
					0.5f, 0.5f, 0.5f, 1);

	light_matrix = offset_scale * light_matrix;

	int view_projection_uniform = 
		glGetUniformLocation(m_diffuse_shadowed_program_id, "view_projection");
	glUniformMatrix4fv(view_projection_uniform, 1, GL_FALSE, (float*) &m_Camera.view_Projection);

	int light_dir_uniform =
		glGetUniformLocation(m_diffuse_shadowed_program_id, "light_dir");
	glUniform3fv(light_dir_uniform, 1, (float*)&light_dir);

	int light_mat_uniform = 
		glGetUniformLocation(m_diffuse_shadowed_program_id, "light_matrix");
	glUniformMatrix4fv(light_mat_uniform, 1, GL_FALSE, (float*)&light_matrix);

	int shadowmap_location = 
		glGetUniformLocation(m_diffuse_shadowed_program_id, "shadow_map");
	glUniform1i(shadowmap_location, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo_depth);

	glBindVertexArray(m_bunny.m_VAO);
	glDrawElements(GL_TRIANGLES, m_bunny.m_indexCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(m_plane.m_VAO);
	glDrawElements(GL_TRIANGLES, m_plane.m_indexCount, GL_UNSIGNED_INT, 0);


	Gizmos::draw(m_Camera.projectionTransform, m_Camera.viewTransform);

	glfwSwapBuffers(this->m_pWindow);
	glfwPollEvents();
}
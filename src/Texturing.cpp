#include "Texturing.h"
#include "Vertex.h"
#include "Utility.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include <cstdio>

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

Texturing::Texturing()
{

}

Texturing::~Texturing()
{

}

bool Texturing::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	Gizmos::create();

	LoadTexture("./data/textures/crate.png");
	LoadShader("./data/shaders/TextureVertex.glsl", 0, "./shaders/TextureFragment.glsl", &m_program_id);
	GenerateQuad(5.0f);
	

	m_camera = FlyCamera(60.0f, 10.0f);
	m_camera.SetLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_camera.m_fSensitivity = 3;

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	return true;
}

void Texturing::Shutdown()
{
	glfwDestroyWindow(this->m_pWindow);
	glfwTerminate();
}

bool Texturing::Update()
{
	if (glfwWindowShouldClose(this->m_pWindow) == true)
	{
		return false;
	}

	return true;
}

void Texturing::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program_id);

	int view_projection_uniform = glGetUniformLocation(m_program_id, "projection_view");

	glUniformMatrix4fv(view_projection_uniform, 1, GL_FALSE, (float*)&m_camera.view_Projection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	int diffuse_location = glGetUniformLocation(m_program_id, "diffuse");

	glUniform1i(diffuse_location, 0);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	Gizmos::draw(m_camera.view_Projection);

	glfwSwapBuffers(this->m_pWindow);
	glfwPollEvents();
}

void Texturing::LoadTexture(const char* a_filename)
{
	int width;
	int height;

	int channels;

	// unsigned char* data = stbi_load(a_filename, &width, &height, &channels, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// stbi_image_free(data);
}

void Texturing::GenerateQuad(float a_size)
{
	VertexTexCoord vertex_data[4];

	vertex_data[0].position = vec4(-a_size, 0, -a_size, 1);
	vertex_data[1].position = vec4(-a_size, 0, a_size, 1);
	vertex_data[2].position = vec4(a_size, 0, a_size, 1);
	vertex_data[3].position = vec4(a_size, 0, -a_size, 1);

	vertex_data[0].tex_coord = vec2(0, 0);
	vertex_data[1].tex_coord = vec2(0, 1);
	vertex_data[2].tex_coord = vec2(1, 1);
	vertex_data[3].tex_coord = vec2(1, 0);

	unsigned int index_data[6] = {0, 2, 1, 0, 3, 2};

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTexCoord) * 4, vertex_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //tex coord

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), 0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof (VertexTexCoord), (void*)sizeof(vec4)); //tex coord
}
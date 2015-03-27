#include "ProceduralGeneration.h"
#include "Utility.h"

bool ProceduralGeneration::Startup()
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

	BuildGrid(vec2(128, 128), glm::vec2(256, 256));
	BuildPerlinTexture(glm::ivec2(256, 256), 10, 0.8f);

	LoadShader("./Shaders/perlin_vertex.glsl", 0,
				"./Shaders/perlin_fragment.glsl", &m_program_id);

	return true;
}

void ProceduralGeneration::BuildGrid(vec2 real_dims, glm::ivec2 dims)
{
	//Compute how many vertices we need
	unsigned int vertex_count = (dims.x + 1) * (dims.y + 1);

	//Allocate vertex data
	VertexTexCoord* vertex_data = new VertexTexCoord[vertex_count];

	//Compute how many indices we need
	unsigned int index_count = dims.x * dims.y * 6;

	//Allocate index data
	unsigned int *index_data = new unsigned int[index_count];

	float curr_y = -real_dims.y / 2.0f;
	//Two nested for loops to generate vertex data
	for (int y = 0; y < dims.y + 1; ++y)
	{
		float curr_x = -real_dims.x / 2.0f;
		for (int x = 0; x < dims.x + 1; ++x)
		{
			//Inside we create our points
			vertex_data[y * (dims.x + 1) + x].position = vec4(curr_x, 0, curr_y, 1);
			vertex_data[y * (dims.x + 1) + x].tex_coord = 
				vec2((float)x / (float)dims.x, (float)y / (float)dims.y );

			curr_x += real_dims.x / (float)dims.x;
		}
		curr_y += real_dims.y / (float)dims.y;
	}

	//Generate our index data
	int curr_index = 0;
	for (int y = 0; y < dims.y; ++y)
	{
		for (int x = 0; x < dims.x; ++x)
		{
			//Create our 6 indices here!
			index_data[curr_index++] = y * (dims.x + 1) + x;
			index_data[curr_index++] = (y + 1) * (dims.x + 1) + x;
			index_data[curr_index++] = (y + 1) * (dims.x + 1) + (x + 1);

			index_data[curr_index++] = (y + 1) * (dims.x + 1) + (x + 1);
			index_data[curr_index++] = y * (dims.x + 1) + (x + 1);
			index_data[curr_index++] = y * (dims.x + 1) + x;
		}
	}

	m_plane_mesh.m_indexCount = index_count;
	//create VertexArrayObject, buffers, etc
	glGenVertexArrays(1, &m_plane_mesh.m_VAO);
	glGenBuffers(1, &m_plane_mesh.m_VBO);
	glGenBuffers(1, &m_plane_mesh.m_IBO);

	//Bind and fill buffers
	glBindVertexArray(m_plane_mesh.m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_plane_mesh.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane_mesh.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, 
		sizeof(VertexTexCoord) * vertex_count, vertex_data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int)* index_count, index_data, GL_STATIC_DRAW);

	//Tell OpenGL about our vertex structure
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
							sizeof(VertexTexCoord), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(VertexTexCoord), (void*)sizeof(vec4));

	//Unbind stuff
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Free vertex and index data
	delete[] vertex_data;
	delete[] index_data;
}

void ProceduralGeneration::BuildPerlinTexture(glm::ivec2 dims, int octaves, float persistance)
{
	//Set the scale
	float scale = (1.0f / dims.x) * 3.0f;

	//Allocate memory for perlin data
	m_perlin_data = new float[dims.x * dims.y];

	//Loop through all the pixels
	for (int y = 0; y < dims.y; ++y)
	{
		for (int x = 0; x < dims.x; ++x)
		{
			float amplitude = 1;
			float freq = 1;
			m_perlin_data[y * dims.x + x] = 0;

			//Loop over a number of octaves - however many got passed in
			for (int o = 0; o < octaves; ++o)
			{
				//Call glm::perlin function
				float perlin_sample =
					glm::perlin(vec2(x, y) * scale * freq) * 0.5 + 0.5;
				perlin_sample *= amplitude;
				m_perlin_data[y * dims.x + x] += perlin_sample;

				amplitude *= persistance;
				freq *= 2.0f;
			}
		}
	}

	//Generate OpenGL texture handles
	glGenTextures(1, &m_perlin_texture);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

	//Pass perlin data to the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dims.x, dims.y, 0, 
		GL_RED, GL_FLOAT, m_perlin_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void ProceduralGeneration::Shutdown()
{
	Gizmos::destroy();
	Application::Shutdown();
}

bool ProceduralGeneration::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	float dT = (float)glfwGetTime();
	glfwSetTime(0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

void ProceduralGeneration::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program_id);

	int view_proj_uniform =
		glGetUniformLocation(m_program_id, "view_proj");
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, (float*)&m_Camera.view_Projection);

	int tex_uniform = 
		glGetUniformLocation(m_program_id, "perlin_texture");
	glUniform1i(tex_uniform, 0);

	int scale_uniform =
		glGetUniformLocation(m_program_id, "scale");
	glUniform1i(scale_uniform, 2.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

	glBindVertexArray(m_plane_mesh.m_VAO);
	glDrawElements(GL_TRIANGLES, m_plane_mesh.m_indexCount,
		GL_UNSIGNED_INT, 0);

	// Gizmos::draw(m_Camera.projectionTransform, m_Camera.viewTransform);

	glfwSwapBuffers(this->m_pWindow);
	glfwPollEvents();
}
#include "RenderingGeometryh.h"
#include "GLMHeader.h"
#include "Vertex.h"

bool RenderingGeometry::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	GenerateShader();
	GenerateGrid(10, 10);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	return true;
}

void RenderingGeometry::Shutdown()
{
	glfwDestroyWindow(this->m_pWindow);
	glfwTerminate();
}

bool RenderingGeometry::Update()
{
	if (glfwWindowShouldClose(this->m_pWindow) == true)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void RenderingGeometry::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_camera.view_Projection);

	glUseProgram(m_programID);

	int projection_view_handle = glGetUniformLocation(m_programID, "projectionView");

	if (projection_view_handle >= 0)
	{
		glUniformMatrix4fv(projection_view_handle, 1, false, (float*)&(m_camera.view_Projection));
	}

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(this -> m_pWindow);
	glfwPollEvents();
}

void RenderingGeometry::GenerateGrid(unsigned int rows, unsigned int cols)
{
	Vertex* vertex_array = new Vertex[ (rows +1) * (cols + 1) ];

	for (unsigned int r = 0; r < rows + 1; ++r)
	{
		for (unsigned int c = 0; c < cols + 1; ++ c)
		{
			vec4 pos = vec4((float)c, 0, (float)r, 1);
			vertex_array[c + r * (cols + 1)].position = pos;

			vec4 color = vec4((float)c / (cols + 1), 0, (float)r / (rows + 1), 1);
			vertex_array[c + r * (cols +1)].color = color;

		}

		m_index_count = rows * cols * 6;
		unsigned int * index_array = new unsigned int[rows * cols * 6];

		int index_location = 0;

		for (unsigned int r = 0; r < rows; ++r)
		{
			for (unsigned int c = 0; c < cols; ++c)
			{
				index_array[index_location + 0] = c + r * cols;
				index_array[index_location + 1] = c + (r + 1) * cols;
				index_array[index_location + 2] = c + (r + 2) * cols;

				index_array[index_location + 3] = (c + 1) + r * cols;
				index_array[index_location + 4] = c + (r + 1) * cols;
				index_array[index_location + 5] = (c + 1) + (r + 1) * cols;
			}
		}

		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_IBO);
		glGenVertexArrays(1, &m_VAO);

		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, (cols + 1) * (rows + 1) * sizeof(Vertex),
														vertex_array, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //color

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec4));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, rows * cols * 6 * sizeof(unsigned int),
															index_array, GL_STATIC_DRAW);
		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		delete[] vertex_array;
		delete[] index_array;
	}
}

void RenderingGeometry::GenerateShader()
{
	const char* vs_source = "#version 410\n \
							layout(location=0) in vec4 position;\n \
							layout(location=1) in vec4 color;\n \
							out vec4 out_color;\n \
							uniform mat4 projection_view;\n \
							void main()\n \
							{\n \
							out_color = color;\n \
							gl_Position = projection_view * position;\
							}\n";

	const char* fs_source = "#version 410\n \
							in vec4 out_color\n \
							out vec4 frag_color\n \
							void main()\n \
							{\n \
							frag_color = out_color\n \
							}\n";

	unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertex_shader, 1, &vs_source, 0);
	glCompileShader(vertex_shader);

	glShaderSource(fragment_shader, 1, &fs_source, 0);
	glCompileShader(fragment_shader);

	m_programID = glCreateProgram();

	glAttachShader(m_programID, vertex_shader);
	glAttachShader(m_programID, fragment_shader);

	glLinkProgram(m_programID);

	int success = GL_FALSE;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int log_length = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &log_length);

		char* infolog = new char[log_length];
		glGetProgramInfoLog(m_programID, log_length, 0, infolog);

		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infolog);
		delete[] infolog;

		glDeleteShader(fragment_shader);
		glDeleteShader(vertex_shader);
	}

}
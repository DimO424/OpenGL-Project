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

	Gizmos::create();

	m_camera = FlyCamera(60.0f, 10.0f);
	m_camera.SetLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_camera.m_fSensitivity = 3;

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

void RenderingGeometry::GenerateGrid(unsigned int a_rows, unsigned int a_cols)
{
	//Sets up Vertexes
	Vertex* vertex_array = new Vertex[(a_rows + 1) * (a_cols + 1)];

	for (unsigned int r = 0; r < a_rows + 1; ++r)
	{
		for (unsigned int c = 0; c < a_cols + 1; ++c)
		{
			vec4 position = vec4((float)c, 0, (float)r, 1);
			vertex_array[c + r * (a_cols + 1)].position = position;

			vec4 color = vec4((float)c / (a_cols + 1), 0, (float)r / (a_rows + 1), 1);
			vertex_array[c + r * (a_cols + 1)].color = color;
		}
	}

	m_index_count = a_rows * a_cols * 6;

	//Sets up Indicies
	unsigned int* index_array = new unsigned int[m_index_count];
	int index_location = 0;

	for (unsigned int r = 0; r < a_rows; ++r)
	{
		for (unsigned int c = 0; c < a_cols; ++c)
		{
			//Triangle 1
			index_array[index_location + 0] = c + r * (a_cols + 1);
			index_array[index_location + 1] = c + (r + 1) * (a_cols + 1);
			index_array[index_location + 2] = c + (r + 2) * (a_cols + 1);

			//Triangle 2
			index_array[index_location + 3] = (c + 1) + r * (a_cols + 1);
			index_array[index_location + 4] = c + (r + 1) * (a_cols + 1);
			index_array[index_location + 5] = (c + 1) + (r + 1) * (a_cols + 1);

			index_location += 6;
		}
	}

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (a_cols + 1) * (a_rows + 1) * sizeof(Vertex),
													vertex_array, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //color

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec4));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_index_count * sizeof(unsigned int),
														index_array, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] vertex_array;
	delete[] index_array;
	
}

void RenderingGeometry::GenerateShader()
{
	const char* vs_source = "#version 410\n														"
													"in vec4 Position;\n						"
													"in vec4 Color;\n 							"
													"out vec4 out_Color;\n 					    "
													"uniform mat4 projection_View;\n 		    "
													"void main()\n 							    "
													"{\n 									    "
													"out_Color = Color;\n 					    "
													"gl_Position = projection_View * Position;\n"
													"}\n";
	
	const char* fs_source = "#version 410									\n "
												"in vec4 out_Color;			\n "
												"out vec4 frag_Color;		\n "
												"void main()				\n "
												"{							\n "
												"	frag_Color = out_Color;	\n "
												"}							\n";

	unsigned int vertex_Shader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragment_Shader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertex_Shader, 1, &vs_source, 0);
	glCompileShader(vertex_Shader);

	glShaderSource(fragment_Shader, 1, &fs_source, 0);
	glCompileShader(fragment_Shader);

	m_programID = glCreateProgram();

	glAttachShader(m_programID, vertex_Shader);
	glAttachShader(m_programID, fragment_Shader);

	glLinkProgram(m_programID);

	//Error checking
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

		glDeleteShader(fragment_Shader);
		glDeleteShader(vertex_Shader);
	}

}
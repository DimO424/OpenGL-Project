#include "RenderTargets.h"
#include "Utility.h"

bool RenderTargets::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	GenerateFrameBuffer();
	GeneratePlane();
	LoadShader("./Shaders/TextureVertex.glsl", 0,
			   "./Shaders/TextureFragment.glsl", &m_program_id);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);

	Gizmos::create();

	m_Camera = FlyCamera(1280.0f / 720.0f, 30.0f);
	m_Camera.SetLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_Camera.m_fSensitivity = 3;

	return true;
}

void RenderTargets::Shutdown()
{
	Gizmos::destroy();
	Application::Shutdown();
}

bool RenderTargets::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	float dT = (float)glfwGetTime();
	glfwSetTime(0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::clear();

	Gizmos::addSphere(vec3(0, 5, 0), 0.5f, 12, 12, vec4(1, 1, 0, 1));

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

void RenderTargets::GenerateFrameBuffer()
{
	//Setup and bind a framebuffer
	glGenFramebuffers(1, &m_fbo);

	//Unbind the FBO so that we can render to the back buffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	//Create a texture and bind it
	glGenTextures(1, &m_fbo_texture);
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);

	//Specify texture format for storage
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 512, 512);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Attach it to the framebuffer as the first colour attachment.
	//The FBO MUST still be bound
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fbo_texture, 0);

	//Setup and bind a 24bit depth buffer as a render buffer
	glGenRenderbuffers(1, &m_fbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);

	//While the FBO is still bound
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fbo_depth);

	//While the FBO is still bound
	GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, draw_buffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("FRAMEBUFFER DID NOT GENERATE PROPERLY!!!\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void RenderTargets::GeneratePlane()
{
	float vertex_data[] =
	{
		-9.6f, 0, -5, 1,	0, 0,
		9.6f, 0, -5, 1, 1, 0,
		9.6f, 10.8f, -5, 1, 1, 1,
		-9.6f, 10.8f, -5, 1, 0, 1,
	};

	unsigned int index_data[] = 
	{
		0, 1, 2,
		0, 2, 3,
	};

	m_plane.m_indexCount = 6;

	glGenVertexArrays(1, &m_plane.m_VAO);
	glBindVertexArray(m_plane.m_VAO);

	glGenBuffers(1, &m_plane.m_VBO);
	glGenBuffers(1, &m_plane.m_IBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_plane.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data),
		vertex_data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data),
		index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //Position
	glEnableVertexAttribArray(1); //Texcoord

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(float)* 6, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(float)* 6, (void*)(sizeof(float) * 4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderTargets::Draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, 512, 512);
	glClearColor(0.75f, 0.75f, 0.75f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 world = m_Camera.worldTransform;

	vec4 plane = vec4(0, 0, 1, -5);

	vec3 reflected = glm::reflect(-world[2].xyz(), plane.xyz());
	reflected = glm::normalize(reflected);

	float distance = (glm::dot((plane.xyz() * plane.w) - world[3].xyz(), plane.xyz()))
						/ (glm::dot(plane.xyz(), -world[2].xyz()));

	vec3 inter = world[3].xyz() - world[2].xyz() * distance;

	world[3].xyz = inter - reflected * distance;

	vec3 up = vec3(0, 1, 0);

	world[2].xyz = -reflected;
	world[0].xyz = glm::normalize(glm::cross(world[2].xyz(), up));
	world[1].xyz = glm::normalize(glm::cross(world[0].xyz(), world[2].xyz()));

	mat4 view = glm::inverse(world);
	mat4 projection_view = m_Camera.projectionTransform * view;

	glUseProgram(m_program_id);

	int projection_view_uniform =
		glGetUniformLocation(m_program_id, "projection_view");

	glUniformMatrix4fv(projection_view_uniform, 1, GL_FALSE,
		(float*)&projection_view);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); //This turns off color rendering

	glBindVertexArray(m_plane.m_VAO);
	glDrawElements(GL_TRIANGLES, m_plane.m_indexCount, GL_UNSIGNED_INT, 0);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glDepthFunc(GL_GREATER);

	//Draw our meshes, or gizmos, to the render target
	Gizmos::draw(m_Camera.projectionTransform, view);
	
	glDepthFunc(GL_LESS);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, 1280, 720);
	glClearColor(0.3f, 0.3f, 0.3f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_Camera.projectionTransform, m_Camera.viewTransform);

	glUseProgram(m_program_id);

	int projection_view_reflected_uniform =
		glGetUniformLocation(m_program_id, "projection_view_reflected");

	glUniformMatrix4fv(projection_view_uniform, 1, GL_FALSE,
		(float*)&m_Camera.view_Projection);

	glUniformMatrix4fv(projection_view_reflected_uniform, 1, GL_FALSE,
		(float*)&projection_view);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);

	int diffuse_uniform =
		glGetUniformLocation(m_program_id, "diffuse");
	glUniform1i(diffuse_uniform, 0);

	glBindVertexArray(m_plane.m_VAO);
	glDrawElements(GL_TRIANGLES, m_plane.m_indexCount,
		GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(this->m_pWindow);
	glfwPollEvents();
}
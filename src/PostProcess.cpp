#include "PostProcess.h" 

void PostProcess::GenerateFrameBuffer()
{
	//Setup and bind a framebuffer
	glGenFramebuffers(1, &m_fbo);

	//Unbind the FBO so that we can render to the back buffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	//Create a texture and bind it
	glGenTextures(1, &m_fbo_texture);
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);

	//Specify texture format for storage
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Attach it to the framebuffer as the first colour attachment.
	//The FBO MUST still be bound
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fbo_texture, 0);

	//Setup and bind a 24bit depth buffer as a render buffer
	glGenRenderbuffers(1, &m_fbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1280, 720);

	//While the FBO is still bound
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fbo_depth);

	//While the FBO is still bound
	GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, draw_buffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("FRAMEBUFFER DID NOT GENERATE PROPERLY!!!\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void PostProcess::GenerateScreenSpaceQuad()
{
	vec2 half_texel = 1.0f / vec2(1280, 720) * 0.5f;

	float vertex_data[]
	{
		-1, -1, 0, 1,		 half_texel.x,		half_texel.y,
		 1, -1, 0, 1,	 1 - half_texel.x,		half_texel.y,
		 1,  1, 0, 1,	 1 - half_texel.x, 1 -  half_texel.y,
		-1,  1, 0, 1,		 half_texel.x, 1 -	half_texel.y,
	};

	unsigned int index_data[] = 
	{
		0, 1, 2, 0, 2, 3
	};

	glGenVertexArrays(1, &m_quad.m_VAO);

	glGenBuffers(1, &m_quad.m_VBO);
	glGenBuffers(1, &m_quad.m_IBO);

	glBindVertexArray(m_quad.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_quad.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quad.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);

}

bool PostProcess::Startup()
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

	return true;
}

void PostProcess::Shutdown()
{
	Gizmos::destroy();
	Application::Shutdown();
}

bool PostProcess::Update()
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

void PostProcess::Draw()
{
	Gizmos::draw(m_Camera.projectionTransform, m_Camera.viewTransform);

	glfwSwapBuffers(this->m_pWindow);
	glfwPollEvents();
}
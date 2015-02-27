#include "GPUEmitter.h"
#include "Utility.h"

GPUPointEmitter::GPUPointEmitter() :
m_particles(nullptr), m_max_particles(0), m_position(0),
m_draw_shader(0), m_update_shader(0), m_last_draw_time(0)
{
	m_VAO[0] = 0;
	m_VAO[1] = 0;
	m_VBO[0] = 0;
	m_VBO[1] = 0;
}

GPUPointEmitter::~GPUPointEmitter()
{
	delete[] m_particles;
	glDeleteVertexArrays(2, m_VAO);
	glDeleteBuffers(2, m_VBO);

	//delete the shaders
	glDeleteProgram(m_update_shader);
	glDeleteProgram(m_draw_shader);
}

void GPUPointEmitter::Init(unsigned int a_max_particles, vec4 a_position, float a_emit_rate,
	float a_min_lifespan, float a_max_lifespan, float a_min_velocity, float a_max_velocity,
	float a_start_size, float a_end_size, vec4 a_start_color, vec4 a_end_color)
{
	m_position = a_position;
	m_start_color = a_start_color;
	m_end_color = a_end_color;
	m_start_size = a_start_size;
	m_end_size = a_end_size;
	m_velocity_min = a_min_velocity;
	m_velocity_max = a_max_velocity;
	m_lifespan_min = a_min_lifespan;
	m_lifespan_max = a_min_lifespan;

	m_max_particles = a_max_particles;

	m_particles = new GPUParticle[m_max_particles];
	m_active_buffer = 0;

	CreateBuffer();
	CreatUpdateShader();
	CreateDrawShader();

}

void GPUPointEmitter::CreateBuffer()
{
	//Create OpenGL buffer
	glGenVertexArrays(2, m_VAO);
	glGenBuffers(2, m_VBO);

	//Setup the buffers
	for (unsigned int buffer_index = 0; buffer_index < 2; ++buffer_index)
	{
		glBindVertexArray(m_VAO[buffer_index]);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[buffer_index]);
		glBufferData(GL_ARRAY_BUFFER, m_max_particles *
			sizeof(GPUParticle), m_particles, GL_STREAM_DRAW);

		glEnableVertexAttribArray(0);//Position
		glEnableVertexAttribArray(1);//Velocity
		glEnableVertexAttribArray(2);//Lifetime
		glEnableVertexAttribArray(3);//Lifespan

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(GPUParticle), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
			sizeof(GPUParticle), ((char*)0) + 12);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,
			sizeof(GPUParticle), ((char*)0) + 24);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE,
			sizeof(GPUParticle), ((char*)0) + 28);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GPUPointEmitter::CreatUpdateShader()
{
	// create a shader
	unsigned int vertex_shader;
	LoadShaderType("./Shaders/particle_update_vert.glsl", GL_VERTEX_SHADER, &vertex_shader);

	m_update_shader = glCreateProgram();
	glAttachShader(m_update_shader, vertex_shader);

	const char* outputs[4] = {"updated_postion", "updated_velocity",
							  "updated_lifetime", "updated_lifespan"};

	// specify the data that we will stream back
	glTransformFeedbackVaryings(m_update_shader, 4, outputs, GL_INTERLEAVED_ATTRIBS);

	glLinkProgram(m_update_shader);

	// remove unneeded handles
	glDeleteShader(vertex_shader);

}

void GPUPointEmitter::CreateDrawShader()
{
	LoadShader("./Shaders/gpu_particle_vert.glsl", 
			   "./Shaders/gpu_particle_geom.glsl", 
			   "./Shaders/gpu_particle_frag.glsl",
			   &m_draw_shader);
}

void GPUPointEmitter::Draw(float a_current_time, 
	mat4 a_camera_transform,
	mat4 a_projection_view)
{
	// uniform float delta_time;
	// 
	// uniform vec3 emitter_position;
	// 
	// uniform float min_velocity;
	// uniform float max_velocity;
	// uniform float min_lifespan;
	// uniform float max_lifespan;
	// 
	// uniform float time;

	//Update vertex pass
	glUseProgram(m_update_shader);

	unsigned int delta_uniform = 
		glGetUniformLocation(m_update_shader, "delta_time");
	unsigned int emitter_position_uniform = 
		glGetUniformLocation(m_update_shader, "emitter_position");
	unsigned int min_velocity_uniform = 
		glGetUniformLocation(m_update_shader, "min_velocity");
	unsigned int max_velocity_uniform = 
		glGetUniformLocation(m_update_shader, "max_velocity");
	unsigned int min_lifespan_uniform = 
		glGetUniformLocation(m_update_shader, "min_lifespan");
	unsigned int max_lifespan_uniform = 
		glGetUniformLocation(m_update_shader, "max_lifespan");
	unsigned int time_uniform = 
		glGetUniformLocation(m_update_shader, "time");

	glUniform1f(delta_uniform, a_current_time - m_last_draw_time);

	glUniform3fv(emitter_position_uniform, 1, (float*)&m_position);
	glUniform1f(min_velocity_uniform, m_velocity_min);
	glUniform1f(max_velocity_uniform, m_velocity_max);
	glUniform1f(min_lifespan_uniform, m_lifespan_min);
	glUniform1f(max_lifespan_uniform, m_lifespan_max);
	glUniform1f(time_uniform, a_current_time);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(m_VAO[m_active_buffer]);

	unsigned int other_buffer = (m_active_buffer + 1) % 2;

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_VBO[other_buffer]);

	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, m_max_particles);

	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK, 0, 0);

	//Render pass
	glUseProgram(m_draw_shader);

	int projection_view_uniform = 
		glGetUniformLocation(m_draw_shader, "projection_view");
	int camera_world_uniform =
		glGetUniformLocation(m_draw_shader, "camera_world");
	int start_size_uniform =
		glGetUniformLocation(m_draw_shader, "start_size");
	int end_size_uniform =
		glGetUniformLocation(m_draw_shader, "end_size");
	int start_color_uniform =
		glGetUniformLocation(m_draw_shader, "start_color");
	int end_color_uniform =
		glGetUniformLocation(m_draw_shader, "end_color");

	glUniformMatrix4fv(projection_view_uniform, 1, GL_FALSE,
										(float*)&a_projection_view);
	glUniformMatrix4fv(camera_world_uniform, 1, GL_FALSE,
										(float*)&a_camera_transform);
	glUniform1f(start_size_uniform, m_start_size);
	glUniform1f(end_size_uniform, m_end_size);
	glUniform4fv(start_color_uniform, 1, (float*)&m_start_color);
	glUniform4fv(end_color_uniform, 1, (float*)&m_end_color);
	
	glBindVertexArray(m_VAO[other_buffer]);
	glDrawArrays(GL_POINTS, 0,m_max_particles);

	m_active_buffer = other_buffer;

	m_last_draw_time = a_current_time;
}


#include "Emitter.h"
#include "gl_core_4_4.h"

Emitter::Emitter() :
	m_particles(nullptr),
	m_max_particles(0),
	m_alive_count(0), 
	m_vertex_data(nullptr),
	m_index_data(nullptr)
{
	m_buffers = {};
}

Emitter::~Emitter()
{
	delete[] m_particles;
	delete[] m_vertex_data;
	delete[] m_index_data;

	glDeleteVertexArrays(1, &m_buffers.m_VAO);
	glDeleteBuffers(1, &m_buffers.m_VBO);
	glDeleteBuffers(1, &m_buffers.m_IBO);
}

void Emitter::Init(unsigned int a_max_particles, vec4 a_position, float a_emit_rate,
	float a_min_lifespan, float a_max_lifespan, float a_min_velocity, float a_max_velocity,
	float a_start_size, float a_end_size, vec4 a_start_color, vec4 a_end_color)
{
	m_position = a_position;
	m_lifespan_min = a_min_lifespan;
	m_lifespan_max = a_max_lifespan;
	m_velocity_min = a_min_velocity;
	m_velocity_max = a_max_velocity;
	m_start_size = a_start_size;
	m_end_size = a_end_size;
	m_start_color = a_start_color;
	m_end_color = a_end_color;

	m_emit_rate = 1.0f / a_emit_rate;

	m_max_particles = a_max_particles;
	m_alive_count = 0;

	//Create particle array
	m_particles = new Particle[m_max_particles];

	//Create the array of vertices for the particles
	//4 vertices per particle for a quad
	m_vertex_data = new VertexParticle[m_max_particles * 4];

	//Create the index buffer data for the particles
	//6 indices per quad of 2 triangles
	m_index_data = new unsigned int[m_max_particles * 6];

	for (unsigned int i = 0; i < m_max_particles; ++i)
	{
		unsigned int start = 4 * i;

		m_index_data[i * 6] = start;
		m_index_data[i * 6 + 1] = start + 1;
		m_index_data[i * 6 + 2] = start + 2;

		m_index_data[i * 6 + 3] = start + 0;
		m_index_data[i * 6 + 4] = start + 2;
		m_index_data[i * 6 + 5] = start + 3;
	}

	//Create OpenGL buffers
	glGenVertexArrays(1, &m_buffers.m_VAO);
	glGenBuffers(1, &m_buffers.m_VBO);
	glGenBuffers(1, &m_buffers.m_IBO);

	glBindVertexArray(m_buffers.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_max_particles * 4 * sizeof(VertexParticle),
												m_vertex_data, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_max_particles * 6 * sizeof(unsigned int),
												m_index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //Pos
	glEnableVertexAttribArray(1); //Col

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexParticle), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexParticle),
												(void*)sizeof(vec4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Emitter::EmitParticles()
{
	unsigned int particles_to_emit = (unsigned int)(m_emit_timer / m_emit_rate);
	m_emit_timer -= particles_to_emit * m_emit_rate;

	for (unsigned int i = 0; i < particles_to_emit && m_alive_count < m_max_particles; ++i)
	{
		m_particles[m_alive_count].position = m_position;
		m_particles[m_alive_count].lifetime = 0;
		m_particles[m_alive_count].lifespan = glm::linearRand(m_lifespan_min, m_lifespan_max);

		m_particles[m_alive_count].color = m_start_color;
		m_particles[m_alive_count].size = m_start_size;

		float velocity_length = glm::linearRand(m_velocity_min, m_velocity_max);
		m_particles[m_alive_count].velocity.xyz = glm::sphericalRand(velocity_length);
		m_particles[m_alive_count].velocity.w = 0;

		m_alive_count++;
	}
}

void Emitter::Update(float a_deltaTime)
{
	//Clean up the dead particles
	for (unsigned int i = 0; i < m_alive_count; ++i)
	{
		m_particles[i].lifetime += a_deltaTime;
		if (m_particles[i].lifetime > m_particles[i].lifespan)
		{
			--m_alive_count;
			m_particles[i] = m_particles[m_alive_count];
			--i;
		}
	}

	m_emit_timer += a_deltaTime;
	EmitParticles();

	//Debug information for FPS and particle count
	m_print_timer += a_deltaTime;

	if (m_print_timer > 1.0f)
	{
		printf("FPS: %f. Particles: %d. \n", 1 / a_deltaTime, m_alive_count);
		m_print_timer = 0.0f;
	}


	//Move all the alive particles
	for (unsigned int i = 0; i < m_alive_count; ++i)
	{
		m_particles[i].position += a_deltaTime * m_particles[i].velocity;

		float t = m_particles[i].lifetime / m_particles[i].lifespan;

		m_particles[i].color = glm::mix(m_start_color, m_end_color, t);
		m_particles[i].size = glm::mix(m_start_size, m_end_size, t);
	}
}

void Emitter::UpdateVertexData(vec3 a_camera_position, vec3 a_camera_up)
{
	for (unsigned int i = 0; i < m_alive_count; ++i)
	{
		int itimes4 = i * 4;

		mat4 particle_transform;

		vec3 to = a_camera_position - m_particles[i].position.xyz;
		vec3 forward = glm::normalize(to);
		vec3 right = glm::cross(a_camera_up, forward);
		vec3 up = glm::cross(forward, right);

		forward *= m_particles[i].size;
		right *= m_particles[i].size;
		up *= m_particles[i].size;

		particle_transform[0].xyz = right;
		particle_transform[1].xyz = up;
		particle_transform[2].xyz = forward;
		particle_transform[3] = m_particles[i].position;

		m_vertex_data[itimes4].position = particle_transform * vec4(-1, 1, 0, 1);
		m_vertex_data[itimes4 + 1].position = particle_transform * vec4(-1, -1, 0, 1);
		m_vertex_data[itimes4 + 2].position = particle_transform * vec4(1, -1, 0, 1);
		m_vertex_data[itimes4 + 3].position = particle_transform * vec4(1, 1, 0, 1);

		m_vertex_data[itimes4].color = m_particles[i].color;
		m_vertex_data[itimes4 + 1].color = m_particles[i].color;
		m_vertex_data[itimes4 + 2].color = m_particles[i].color;
		m_vertex_data[itimes4 + 3].color = m_particles[i].color;
	}
}

void Emitter::Render()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_alive_count * 4 * sizeof(VertexParticle), m_vertex_data);

	glBindVertexArray(m_buffers.m_VAO);
	glDrawElements(GL_TRIANGLES, 6 * m_alive_count, GL_UNSIGNED_INT, 0);
}

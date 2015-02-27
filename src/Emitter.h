#ifndef _EMITTER_H_
#define _EMITTER_H_

#include "GLMHeader.h"
#include "Vertex.h"

struct Particle
{
	vec4 color;
	vec4 position;
	vec4 velocity;
	float size;
	float lifetime;
	float lifespan;
};

// What does an emitter do?
// It contains all of the particles it conrols
// It spawns new particles
// it updates existing particles
// it renderes its particles

class Emitter
{
public:
	Emitter();
	~Emitter();

	void Init(unsigned int a_max_particles, vec4 a_position, float a_emit_rate, 
			  float a_min_lifespan, float a_max_lifespan, float a_min_velocity, float a_max_velocity,
			  float a_start_size, float a_end_size, vec4 a_start_color, vec4 a_end_color);

	void EmitParticles();
	void Update(float a_deltaTime);
	void Render();

	void UpdateVertexData(vec3 a_camera_position, vec3 a_camera_up);

	// Particle data
	Particle *m_particles;
	unsigned int m_max_particles;
	unsigned int m_alive_count;

	//OpenGL data
	OpenGLData m_buffers;
	VertexParticle* m_vertex_data;
	unsigned int* m_index_data;

	//Emitter data
	vec4 m_position;

	float m_emit_rate;
	float m_emit_timer;
	float m_print_timer;

	float m_lifespan_min;
	float m_lifespan_max;

	float m_velocity_min;
	float m_velocity_max;

	float m_start_size;
	float m_end_size;

	vec4 m_start_color;
	vec4 m_end_color;
};

#endif
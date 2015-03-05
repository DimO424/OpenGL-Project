#ifndef _GPUEMITTER_H_
#define _GPUEMITTER_H_

#include "GLMHeader.h"
#include "Vertex.h"

//gpu particle struct
struct GPUParticle
{
	GPUParticle() : position(vec3(0)), velocity(vec3(0)), lifespan(0), lifetime(1) {}
	vec3 position;
	vec3 velocity;
	float lifetime;
	float lifespan;
};

//gpu emitter class
	//creating gpu buffers
	//setting up stuff for transform feedback
	//creating the shaders
	//actually rendering them

class GPUPointEmitter
{
public:
	GPUPointEmitter();
	~GPUPointEmitter();

	void Init(unsigned int a_max_particles, vec4 a_position, float a_emit_rate,
			  float a_min_lifespan, float a_max_lifespan, float a_min_velocity, float a_max_velocity,
			  float a_start_size, float a_end_size, vec4 a_start_color, vec4 a_end_color);

	void Draw(float a_current_time, mat4 a_camera_transform, mat4 a_projection_view);

	void CreateBuffer();
	void CreatUpdateShader();
	void CreateDrawShader();

	GPUParticle* m_particles;
	unsigned int m_max_particles;

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

	unsigned int m_active_buffer;
	unsigned int m_VAO[2];
	unsigned int m_VBO[2];
	unsigned int m_update_shader;
	unsigned int m_draw_shader;
	float m_last_draw_time;

	//General data
	float m_emit_rate_multiplier;
};

#endif
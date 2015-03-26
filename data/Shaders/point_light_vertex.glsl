#version 410

layout (location = 0) in vec4 position;

//Position in world-space
uniform vec3 lightPosition;
uniform float light_radius;

uniform mat4 proj_view;

void main()
{
	gl_Position = proj_view *
		  vec4(position.xyz * 
				light_radius + lightPosition, 1);
}
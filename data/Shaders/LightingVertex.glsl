#version 410

layout(location=0) in vec4 Position;
layout(location=1) in vec4 Normal;

out vec4 frag_normal;
out vec4 frag_position;

uniform mat4 projection_view;

void main()
{
	frag_position = Position;
	frag_normal = Normal;
	gl_Position = projection_view * Position;

}
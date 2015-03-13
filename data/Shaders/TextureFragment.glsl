#version 410

// in vec2 frag_tex_coord;

in vec4 reflected_screen_position;

out vec4 frag_color;
uniform sampler2D diffuse;

void main()
{
	// frag_color = texture(diffuse, frag_tex_coord);

	vec4 uv_position = reflected_screen_position / reflected_screen_position.w;

	uv_position = (uv_position + 1) * 0.5f;
	frag_color = texture(diffuse, uv_position.xy);
};
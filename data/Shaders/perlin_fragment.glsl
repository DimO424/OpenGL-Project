#version 410

in vec2 frag_texcoord;

out vec4 frag_color;

uniform sampler2D perlin_texture;

void main()
{
	vec4 black = vec4(0, 0, 0, 1);
	vec4 blue = vec4(0, 0, 1, 1);
	vec4 yellow = vec4(1, 1, 0, 1);
	vec4 green = vec4(0, 1, 0, 1);
	vec4 red = vec4(1, 0, 0, 1);
	vec4 white = vec4(1);

	if (perlin_texture < 1)
	{ 
		frag_color = blue;
	}

	// frag_color =
		// texture(perlin_texture, frag_texcoord).rrrr;
	else
	{
		frag_color = 1;
	}
}
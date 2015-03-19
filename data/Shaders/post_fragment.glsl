#version 410

in vec2 frag_texcoord;

out vec4 frag_color;

uniform sampler2D input_texture;

vec4 BoxBlur()
{
	vec2 texel_uv = 1.0f / textureSize(input_texture, 0).xy;

	vec4 color = texture(input_texture, frag_texcoord);

	color += texture(input_texture, frag_texcoord + texel_uv);

	color += texture(input_texture, frag_texcoord + vec2 (texel_uv.x, 0));
	color += texture(input_texture, frag_texcoord + vec2 (texel_uv.x, -texel_uv.y));
	color += texture(input_texture, frag_texcoord + vec2 (0, texel_uv.y));
														 		    
	color += texture(input_texture, frag_texcoord + vec2 (0, -texel_uv.y));
	color += texture(input_texture, frag_texcoord + vec2 (-texel_uv.x, texel_uv.y ));
	color += texture(input_texture, frag_texcoord + vec2 (-texel_uv.x, 0));
	color += texture(input_texture, frag_texcoord + vec2 (-texel_uv.x, -texel_uv.y));

	color /= 9;

	return color;
}

vec4 GausianBlur()
{
	vec2 texel_uv = 1.0f / textureSize(input_texture, 0).xy;

	float quarter = 0.25f;
	float eighth = 1.0f / 8.0f;
	float sixteenth = 1.0f / 16.0f; 

	vec4 color = texture(input_texture, frag_texcoord);

	color += texture(input_texture, frag_texcoord + texel_uv);

	color += texture(input_texture, frag_texcoord + vec2 (texel_uv.x, 0));
	color += texture(input_texture, frag_texcoord + vec2 (texel_uv.x, -texel_uv.y));
	color += texture(input_texture, frag_texcoord + vec2 (0, texel_uv.y));
														 		    
	color += texture(input_texture, frag_texcoord + vec2 (0, -texel_uv.y));
	color += texture(input_texture, frag_texcoord + vec2 (-texel_uv.x, texel_uv.y ));
	color += texture(input_texture, frag_texcoord + vec2 (-texel_uv.x, 0));
	color += texture(input_texture, frag_texcoord + vec2 (-texel_uv.x, -texel_uv.y));

	color /= 9;

	return color;
}

vec4 Distort()
{
	vec2 mid = vec2(0.5f, 0.5f);

	float distance_from_centre = distance(frag_texcoord, mid);
	vec2 normalized_coord = normalize(frag_texcoord - mid);

	float bias = distance_from_centre +
				 sin(distance_from_centre * 30) * 0.01f;

	vec2 new_coord = mid + bias * normalized_coord;

	return texture(input_texture, new_coord);
}

void main()
{
	// frag_color = texture(input_texture, frag_texcoord);
	// frag_color = vec4(frag_texcoord, 0, 1);

	frag_color = Distort();
};
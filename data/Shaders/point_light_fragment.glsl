#version 410

out vec3 light_output;

uniform vec3 light_view_position;
uniform vec3 light_diffuse;
uniform float light_radius;

uniform sampler2D position_texture;
uniform sampler2D normal_texture;

void main()
{
	//Calculate the texture coordinates for the fragment
	vec2 texcoord = gl_FragCoord.xy /
					textureSize(position_texture, 0).xy;

	//Sample from textures
	vec3 position_sample = texture(position_texture, texcoord).xyz;
	vec3 normal_sample = texture(normal_texture, texcoord).xyz;
	normal_sample *= 2;
	normal_sample -= 1;

	//Compute light direction
	vec3 to_light = light_view_position - position_sample;
	vec3 L = normalize(to_light);

	//Compute the lambertian term
	float d = max(0, dot(normal_sample, L));

	//Compute the falloff
	float falloff = 1 - min(1, length(to_light) / light_radius);

	//Output lambert * falloff * color
	light_output = d * falloff * light_diffuse;
}

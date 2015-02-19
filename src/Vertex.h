#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "GLMHeader.h"

struct Vertex
{
	vec4 position;
	vec4 color;
};

struct VertexTexCoord
{
	vec4 position;
	vec2 tex_coord;
};

struct VertexNormal
{
	vec4 position;
	vec4 normal;
	vec4 tangent;
	vec4 texCoord;
};

#endif
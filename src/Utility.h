#ifndef UTILITY_H_
#define UTILITY_H_

typedef unsigned int GLuint;

bool LoadShaderType(char* a_filename, GLuint a_shader_type, unsigned int* a_output);

bool LoadShader(char* a_vertex_filename,
				char* a_geometry_filename,
				char* a_fragment_filename,
				GLuint* a_result);

#endif
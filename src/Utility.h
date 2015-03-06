#ifndef UTILITY_H_
#define UTILITY_H_

#include "GLMHeader.h"

#include "AntTweakBar.h"

typedef unsigned int GLuint;

bool LoadShaderType(char* a_filename, GLuint a_shader_type, unsigned int* a_output);

bool LoadShader(char* a_vertex_filename,
				char* a_geometry_filename,
				char* a_fragment_filename,
				GLuint* a_result);

//AntTweakBar helper functions
void OnMouseButton(GLFWwindow* window, int button, int pressed, int mod_keys);
void OnMousePosition(GLFWwindow* window, double x, double y);
void OnMouseScroll(GLFWwindow* window, double x, double y);
void OnKey(GLFWwindow* window, int key, int scancode, int pressed, int mod_keys);
void OnChar(GLFWwindow* window, unsigned int c);
void OnWindowResize(GLFWwindow* window, int width, int height);

#endif
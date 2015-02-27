#include "Utility.h"
#include "GLMHeader.h"
#include <cstdio>

bool LoadShaderType(char* a_filename, GLuint a_shader_type, unsigned int* a_output)
{
	bool succeeded = true;

	FILE* shader_file = fopen(a_filename, "r");

	if (shader_file == 0)
	{
		succeeded = false;
	}

	else
	{
		fseek(shader_file, 0, SEEK_END);
		int shader_file_length = ftell(shader_file);
		fseek(shader_file, 0, SEEK_SET);

		char* shader_source = new char[shader_file_length];

		shader_file_length = fread(shader_source, 1, shader_file_length, shader_file);

		unsigned int shader_handle = glCreateShader(a_shader_type);

		glShaderSource(shader_handle, 1, &shader_source, &shader_file_length);
		glCompileShader(shader_handle);

		int success = GL_FALSE;

		glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			int log_length = 0;
			glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &log_length);

			char* infolog = new char[log_length];
			glGetShaderInfoLog(shader_handle, log_length, 0, infolog);

			printf("Error: Failed to link shader program!\n");
			printf("%s\n", infolog);

			delete[] infolog;
			succeeded = false;
		}

		if (succeeded)
		{
			*a_output = shader_handle;
		}

		delete[] shader_source;
		fclose(shader_file);
	}
	return succeeded;
}

bool LoadShader(char* a_vertex_filename,
				char* a_geometry_filename,
				char* a_fragment_filename,
				GLuint* a_result)
{
	bool succeeded = true;

	*a_result = glCreateProgram();

	unsigned int vertex_shader;
	if (LoadShaderType(a_vertex_filename, GL_VERTEX_SHADER, &vertex_shader))
	{
		glAttachShader(*a_result, vertex_shader);
		glDeleteShader(vertex_shader);
	}
	else
	{
		printf("Error: Failed to load VERTEX SHADER!\n");
	}

	if (a_geometry_filename != nullptr)
	{
		unsigned int geometry_shader;
		if (LoadShaderType(a_geometry_filename, GL_VERTEX_SHADER, &geometry_shader))
		{
			glAttachShader(*a_result, geometry_shader);
			glDeleteShader(geometry_shader);
		}
		else
		{
			printf("Error: Failed to load GEOMETRY SHADER!\n");
		}
	}

	if (a_fragment_filename != nullptr)
	{
		unsigned int fragment_shader;
		if (LoadShaderType(a_fragment_filename, GL_VERTEX_SHADER, &fragment_shader))
		{
			glAttachShader(*a_result, fragment_shader);
			glDeleteShader(fragment_shader);
		}
		else
		{
			printf("Error: Failed to load FRAGMENT SHADER!\n");
		}
	}

	glLinkProgram(*a_result);

	GLint success = GL_FALSE;

	glGetProgramiv(*a_result, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		GLint log_length = 0;
		glGetProgramiv(*a_result, GL_INFO_LOG_LENGTH, &log_length);

		char* infolog = new char[log_length];
		glGetProgramInfoLog(*a_result, log_length, 0, infolog);

		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infolog);

		delete[] infolog;
		succeeded = false;
	}

	return succeeded;
}
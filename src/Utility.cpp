#include "Utility.h"
#include "GLMHeader.h"
#include <cstdio>

bool LoadShaderType(char* a_filename, GLuint a_shader_type, unsigned int* a_output)
{
	bool succeeded = true;

	//Open shader file
	FILE* shader_file = fopen(a_filename, "r");

	//Did it open successfully
	if (shader_file == 0)
	{
		printf("ERROR: Failed to open the \"%s\" shader file!!\n", a_filename);
		succeeded = false;
	}
	else
	{
		//Find out how long the file is
		fseek(shader_file, 0, SEEK_END);
		int shader_file_length = ftell(shader_file);
		fseek(shader_file, 0, SEEK_SET);

		//Allocate space for the file
		char* shader_source = new char[shader_file_length];

		//Read the file and update the length to be accurate
		shader_file_length = fread(shader_source, 1, shader_file_length, shader_file);
		succeeded = true;

		//Create the shader based on the type passed in
		unsigned int shader_handle = glCreateShader(a_shader_type);

		//Compile the shader
		glShaderSource(shader_handle, 1, &shader_source, &shader_file_length);
		glCompileShader(shader_handle);

		//Error checking now ...
		int success = GL_FALSE;
		glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			//Then we had a problem
			int log_length = 0;
			glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &log_length);
			GLchar* infolog = new char[log_length];
			glGetShaderInfoLog(shader_handle, log_length, 0, infolog);

			printf("ERROR: Shader compile failed!!\nFile: %s\nError Log:\n", a_filename);
			printf("%s\n", infolog);

			delete[] infolog;
			succeeded = false;
		}

		//Only give the result to the caller if we succeeded
		if (succeeded)
		{
			*a_output = shader_handle;
		}

		delete[] shader_source;
	}

	if (shader_file != NULL)
	{
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
	LoadShaderType(a_vertex_filename, GL_VERTEX_SHADER, &vertex_shader);
	glAttachShader(*a_result, vertex_shader);
	glDeleteShader(vertex_shader);

	if (a_geometry_filename != nullptr)
	{
		unsigned int geometry_shader;
		if (LoadShaderType(a_geometry_filename, GL_GEOMETRY_SHADER, &geometry_shader))
		{
			glAttachShader(*a_result, geometry_shader);
			glDeleteShader(geometry_shader);
		}
	}

	if (a_fragment_filename != nullptr)
	{
		unsigned int fragment_shader;
		if (LoadShaderType(a_fragment_filename, GL_FRAGMENT_SHADER, &fragment_shader))
		{
			glAttachShader(*a_result, fragment_shader);
			glDeleteShader(fragment_shader);
		}
	}

	glLinkProgram(*a_result);

	GLint success = GL_FALSE;
	glGetProgramiv(*a_result, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		//Then we had a problem
		GLint log_length = 0;
		glGetProgramiv(*a_result, GL_INFO_LOG_LENGTH, &log_length);

		char* infolog = new char[log_length];
		glGetProgramInfoLog(*a_result, log_length, 0, infolog);

		printf("ERROR: Shader compile failed!!\nError Log:\n");
		printf("%s", infolog);

		delete[] infolog;
		succeeded = false;
	}

	return succeeded;
}
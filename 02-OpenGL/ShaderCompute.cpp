#include "ShaderCompute.h"
#include <fstream>
#include <iostream>
#include <vector>

ShaderCompute::ShaderCompute(GLuint* gShaderP)
{
	gShaderProgram = gShaderP;
	compile();
}

ShaderCompute::~ShaderCompute()
{

}

bool ShaderCompute::compile()
{
	const char* compute_shader = R"( 
		#version 430

		struct particles
		{
			vec3 pos;
			float life;
			vec3 velocity;
			float tTrash;
		};

		layout (std430, binding = 2) buffer particlesArray
		{
			particles data[];
		};

		layout (local_size_x = 30, local_size_y = 1, local_size_z = 1)in;

		void main()
		{
			uint tmpID =  gl_GlobalInvocationID.x;
			if( data[tmpID].life >= 40 )
			{
				data[tmpID].pos -= data[tmpID].velocity;
				data[tmpID].life -= 1;
			}
		}
	)";

	GLint success = 0;

	//create compute shader
	GLuint cs = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(cs, 1, &compute_shader, nullptr);
	glCompileShader(cs);
	CompileErrorPrint(&cs);

	//compute shader
	*gShaderProgram = glCreateProgram();
	glAttachShader(*gShaderProgram, cs);
	glLinkProgram(*gShaderProgram);
	LinkErrorPrint(gShaderProgram);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		printf("Error");

	return true;
}

void ShaderCompute::LinkErrorPrint(GLuint* shaderProgram)
{
	GLint success;
	glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(*shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(*shaderProgram, maxLength, &maxLength, &errorLog[0]);

		std::fstream myfile;
		myfile.open("errorLinkCheck.txt", std::fstream::out);
		for (int i = 0; i < maxLength; i++)
		{
			myfile << errorLog[i];
		}
		myfile.close();

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteProgram(*shaderProgram); // Don't leak the shader.
		throw;
	}
}

void ShaderCompute::CompileErrorPrint(GLuint* shader)
{
	GLint success;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv((*shader), GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog((*shader), maxLength, &maxLength, &errorLog[0]);

		std::fstream myfile;
		myfile.open("errorCheck.txt", std::fstream::out);
		for (int i = 0; i < maxLength; i++)
		{
			myfile << errorLog[i];
		}
		myfile.close();

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(*shader); // Don't leak the shader.
		throw;
	}
}
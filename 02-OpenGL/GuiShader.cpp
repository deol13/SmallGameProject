#include "GuiShader.h"
#include <vector>
#include <fstream>

GuiShader::GuiShader(){}
GuiShader::GuiShader(GLuint* gShaderP)
{
	gShaderProgram = gShaderP;
	compile();
	mapSampler = glGetUniformLocation(*gShaderProgram, "textureSample");
}
GuiShader::~GuiShader()
{}

bool GuiShader::compile()
{
	const char* vertex_shader = R"(
		#version 430
		layout(location = 0) in vec3 Position;
		layout(location = 1) in vec2 UV;

		out vec2 UV0; 

		void main () 
		{
			UV0 = UV;
			gl_Position = vec4(Position, 1.0f);
		}

	)";

	const char* fragment_shader = R"(
		#version 430
		in vec2 UV0;

		uniform sampler2D textureSample;

		out vec4 fragment_color;

		void main () 
		{
			fragment_color = texture(textureSample, UV0);
		}
	)";

	GLint success = 0;

	//create vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, nullptr);
	glCompileShader(vs);
	CompileErrorPrint(&vs);

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, nullptr);
	glCompileShader(fs);
	CompileErrorPrint(&fs);

	*gShaderProgram = glCreateProgram();
	glAttachShader(*gShaderProgram, vs);
	glAttachShader(*gShaderProgram, fs);
	glLinkProgram(*gShaderProgram);
	LinkErrorPrint(gShaderProgram);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		printf("Error");
	return true;
}

void GuiShader::LinkErrorPrint(GLuint* gShaderProgram)
{
	GLint success;
	glGetProgramiv(*gShaderProgram, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(*gShaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(*gShaderProgram, maxLength, &maxLength, &errorLog[0]);

		std::fstream myfile;
		myfile.open("errorLinkCheck.txt", std::fstream::out);
		for (int i = 0; i < maxLength; i++)
		{
			myfile << errorLog[i];
		}
		myfile.close();

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteProgram(*gShaderProgram); // Don't leak the shader.
		throw;
	}
}
void GuiShader::CompileErrorPrint(GLuint* shader)
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
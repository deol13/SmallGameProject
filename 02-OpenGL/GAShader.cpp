#include "GAShader.h"
#include <vector>
#include <fstream>

GAShader::GAShader(){}
GAShader::GAShader(GLuint* gShaderP)
{
	gShaderProgram = gShaderP;
	compile();
	ViewMatrix = glGetUniformLocation(*gShaderProgram, "ViewMatrix");
	ProjectionMatrix = glGetUniformLocation(*gShaderProgram, "ProjectionMatrix");
	worldMatrix = glGetUniformLocation(*gShaderProgram, "WorldMatrix");

}
GAShader::~GAShader()
{
	delete gShaderProgram;
}


bool GAShader::compile()
{
	const char* vertex_shader = R"(
		#version 430
		layout(location = 0) in vec3 GAPosition;
		layout(location = 1) in vec2 GATex;

		uniform mat4 ViewMatrix;
		uniform mat4 WorldMatrix;

		out vec2 texCoords;

		void main () 
		{
			gl_Position = ViewMatrix * WorldMatrix * vec4(GAPosition, 1.0f);
			texCoords = GATex;
		}

	)";

	const char* geometry_shader = R"(
		#version 430
		layout (triangles) in;
		layout (triangle_strip) out;
		layout(max_vertices = 3) out;

		in vec2 texCoords[];

		uniform mat4 ProjectionMatrix;
		
		out vec2 texCoordsGeo;

		void main ()
		{
			for ( int i = 0; i < gl_in.length(); i++ )
			{
				gl_Position = ProjectionMatrix * gl_in[i].gl_Position;
				texCoordsGeo = texCoords[i];
				EmitVertex();
			}
			EndPrimitive();
		}
	)";

	const char* fragment_shader = R"(
		#version 430
		in vec2 texCoordsGeo;

		uniform sampler2D heightMapSampler;

		out vec4 fragColor;

		void main () 
		{
			fragColor = texture(heightMapSampler, vec2(texCoordsGeo));
		}
	)";

	GLint success = 0;

	//create vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, nullptr);
	glCompileShader(vs);
	CompileErrorPrint(&vs);

	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(gs, 1, &geometry_shader, nullptr);
	glCompileShader(gs);
	CompileErrorPrint(&gs);

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, nullptr);
	glCompileShader(fs);
	CompileErrorPrint(&fs);

	*gShaderProgram = glCreateProgram();
	glAttachShader(*gShaderProgram, vs);
	glAttachShader(*gShaderProgram, gs);
	glAttachShader(*gShaderProgram, fs);
	glLinkProgram(*gShaderProgram);
	LinkErrorPrint(gShaderProgram);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		printf("Error");
	return true;
}


void GAShader::LinkErrorPrint(GLuint* gShaderProgram)
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
void GAShader::CompileErrorPrint(GLuint* shader)
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
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
	attackColor = glGetUniformLocation(*gShaderProgram, "attackColor");
}
GAShader::~GAShader()
{
}


bool GAShader::compile()
{
	const char* vertex_shader = R"(
		#version 430
		layout(location = 0) in vec3 GAPosition;
		layout(location = 1) in vec2 GATex;

		uniform mat4 WorldMatrix;

		out vec2 texCoords;

		void main () 
		{
			gl_Position = WorldMatrix * vec4(GAPosition, 1.0f);
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
		uniform mat4 ViewMatrix;
		
		out vec2 texCoordsGeo;
		out vec3 modelViewPos;
		out vec3 normalWorld;

		void main ()
		{
			vec3 v1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
			vec3 v2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

			for ( int i = 0; i < gl_in.length(); i++ )
			{
				modelViewPos = gl_in[i].gl_Position.xyz;
				gl_Position = ProjectionMatrix * ViewMatrix * gl_in[i].gl_Position;
				texCoordsGeo = texCoords[i];
				normalWorld = normalize(cross(v1, v2));
				EmitVertex();
			}
			EndPrimitive();
		}
	)";

	const char* fragment_shader = R"(
		#version 430
		in vec2 texCoordsGeo;
		in vec3 modelViewPos;
		in vec3 normalWorld;

		uniform sampler2D heightMapSampler;
		uniform int attackColor;

		layout (location = 0) out vec3 WorldPosOut;   
		layout (location = 1) out vec3 DiffuseOut;     
		layout (location = 2) out vec3 NormalOut;     

		void main () 
		{
			WorldPosOut = modelViewPos;
			DiffuseOut = texture(heightMapSampler, texCoordsGeo).xyz;
			NormalOut = normalWorld;
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
	if(error != GL_NO_ERROR)
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
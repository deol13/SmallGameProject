#ifndef GASHADER_H
#define GASHADER_H

#include <gl/glew.h>
#include <gl/GL.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

class GAShader
{
	public:
		GAShader();
		GAShader(GLuint* gShaderP);
		~GAShader();

		GLuint* gShaderProgram;
		bool compile();

		GLint ViewMatrix;
		GLint ProjectionMatrix;
		GLint worldMatrix;
		GLint attackColor;

	private:
		void CompileErrorPrint(GLuint* shader);
		void LinkErrorPrint(GLuint* gShaderProgram);
};


#endif
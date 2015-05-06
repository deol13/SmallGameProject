#ifndef GUISHADER_H
#define GUISHADER_H

#include <gl/glew.h>
#include <gl/GL.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>


class GuiShader
{
private:
	void CompileErrorPrint(GLuint* shader);
	void LinkErrorPrint(GLuint* gShaderProgram);

public:
	GuiShader();
	GuiShader(GLuint* gShaderP);
	~GuiShader();

	GLuint* gShaderProgram;
	GLint mapSampler;

	bool compile();
};

#endif
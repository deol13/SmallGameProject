#ifndef MAPEDITSHADER_H
#define MAPEDITSHADER_H

#include <gl/glew.h>
#include <gl/GL.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

class MapEditShader
{
private:
	void CompileErrorPrint(GLuint* shader);
	void LinkErrorPrint(GLuint* gShaderProgram);
public:
	MapEditShader();
	MapEditShader(GLuint* gShaderP);
	~MapEditShader();

	GLuint* gShaderProgram;
	bool compile();

	GLint ViewMatrix;
	GLint ProjectionMatrix;
	GLint worldMatrix;
	GLint mapSampler;
};

#endif
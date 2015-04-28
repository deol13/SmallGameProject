#ifndef LIGHTSHADER_H
#define LIGHTSHADER_H

#include <gl/glew.h>
#include <gl/GL.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "SpotLight.h"

class LightShader
{
private:
	GLuint* gShaderProgram;
	bool compile();
public:
	LightShader();
	LightShader(GLuint* gShaderP);
	~LightShader();

	//GBUFFER TEXTURES
	GLint Use;
	GLint Position;
	GLint Diffuse;
	GLint Normal;
	GLint UVcord;
	GLint Depth;

	//Number of ...
	GLint NumSpotLights;
	GLint NumSpotLightsShadow;

	//Shadow map
	GLint ShadowMaps;

	//Shadow map matrices
	GLint ProjectionMatrixSM;
	GLint ViewMatrixSM;
	GLint eyepos;

	//Spot light buffer
	GLuint lightBuffer, lightBlockUniformLoc;
	GLuint bindingPoint = 1;

	void CompileErrorPrint(GLuint* shader);
	void LinkErrorPrint(GLuint* gShaderProgram);
};

#endif
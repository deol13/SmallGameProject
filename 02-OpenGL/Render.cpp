#include <fstream>
#include <cstdlib>
#include "Render.h"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

Render::Render()
{
	gShaderGA = 0;
}
Render::Render(int GASIZE)
{
	viewMatrix = glm::lookAt( glm::vec3( -50, 200.0f, -50 ), glm::vec3( GASIZE / 2, 0, GASIZE / 2 ), glm::vec3( 0, 1, 0 ) ); //Test
	//viewMatrix = glm::lookAt(glm::vec3(GASIZE / 2, 200.0f, 30), glm::vec3(GASIZE / 2, 0, GASIZE / 2), glm::vec3(0, 1, 0));
	//viewMatrix = glm::lookAt(glm::vec3(0, 0, -2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));  //Original
	projMatrix = glm::perspective(70.0f, 640.f / 480.0f, 0.5f, 2000.0f);
	gShaderGA = 0;
}
Render::~Render()
{
	delete ga;
}

void Render::init(int GASIZE)
{
	gaShader = new GAShader(&gShaderGA);
	ga = new GameArea();

	ga->loadImage();
	ga->createGA(GASIZE);
	ga->GABuffers();
	//ga->createIBO();
}

void Render::render()
{
	glUseProgram(gShaderGA);
	glProgramUniformMatrix4fv(gShaderGA, gaShader->ViewMatrix, 1, false, &viewMatrix[0][0]);
	glProgramUniformMatrix4fv(gShaderGA, gaShader->ProjectionMatrix, 1, false, &projMatrix[0][0]);

	glBindVertexArray(ga->gGAAttribute);
	glBindBuffer(GL_ARRAY_BUFFER, ga->gGABuffer);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ga->gIndexBuffer);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ga->imageTex);
	
	//glDrawElements(GL_TRIANGLE_STRIP, ga->getIBOCount(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		printf("Error");
}

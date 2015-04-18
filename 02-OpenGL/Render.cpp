#include <fstream>
#include <cstdlib>
#include "Render.h"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

glm::mat4 worldMatrixMap = glm::mat4(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

Render::Render()
{
	gShaderGA = 0;
}
Render::Render(int GASIZE)
{
	viewMatrix = glm::lookAt( glm::vec3( -20, 20.0f, -20 ), glm::vec3( 0, 0, 0 ), glm::vec3( 0, 1, 0 ) ); //Test
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
	loadTextures();

	//hard coded test object
	//testObj = GObject( "victest.obj", GL_QUADS, textures[ 0 ] );
}

void Render::loadTextures() 
{
	createTexture( "testvic.png" );
}

void Render::createTexture( std::string fileName ) 
{
	GLuint texture;
	int x, y, n;
	std::string filePath = "Resource/" + fileName;
	unsigned char* textureData = stbi_load( filePath.c_str(), &x, &y, &n, 4 );
	glGenTextures( 1, &texture );
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData );
	textures.push_back( texture );
	stbi_image_free( textureData );
}

void Render::render(GuiManager* gui, std::vector<GObject*> renderObjects)
{
	glClearColor(0.1, 0.1, 0.1, 1.0);

	glUseProgram(gShaderGA);
	glProgramUniformMatrix4fv(gShaderGA, gaShader->ViewMatrix, 1, false, &viewMatrix[0][0]);
	glProgramUniformMatrix4fv(gShaderGA, gaShader->ProjectionMatrix, 1, false, &projMatrix[0][0]);
	glProgramUniformMatrix4fv(gShaderGA, gaShader->worldMatrix, 1, false, &worldMatrixMap[0][0]);

	glBindVertexArray(ga->gGAAttribute);
	glBindBuffer(GL_ARRAY_BUFFER, ga->gGABuffer);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Draw Map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ga->imageTex);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//Draw GUI
	gui->render();

	//Draw Player
	glBindTexture(GL_TEXTURE_2D, renderObjects[0]->getTexture());
	renderObjects[0]->render(gaShader->worldMatrix, *gaShader->gShaderProgram);


	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		printf("Error");
}

//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ga->gIndexBuffer);
//glDrawElements(GL_TRIANGLE_STRIP, ga->getIBOCount(), GL_UNSIGNED_INT, 0);
//ga->createIBO();

GLuint Render::getTexture(int index) const
{
	return textures[index];
}

GLuint Render::getGAShader()
{
	return gShaderGA;
}

GLint Render::getWorldMatixLoc()
{
	return gaShader->worldMatrix;
}
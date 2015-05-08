#include <fstream>
#include <cstdlib>
#include "Render.h"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

//glm::mat4 worldMatrixMap = glm::mat4(
//	1.0f, 0.0f, 0.0f, 0.0f,
//	0.0f, 1.0f, 0.0f, 0.0f,
//	0.0f, 0.0f, 1.0f, 0.0f,
//	0.0f, 0.0f, 0.0f, 1.0f);

Render::Render()
{
	gShaderGA = 0;
	onExitCleanUp = false;
}
Render::Render(int GASIZE, float aspectRatio)
{
	//viewMatrix = glm::lookAt( glm::vec3( -20, 20.0f, -20 ), glm::vec3( 0, 0, 0 ), glm::vec3( 0, 1, 0 ) ); //Test
	viewMatrix = glm::lookAt(glm::vec3(GASIZE / 2, 200.0f, GASIZE / 2), glm::vec3(GASIZE / 2, 0, GASIZE / 2), glm::vec3(0, 0, 1));
	//viewMatrix = glm::lookAt(glm::vec3(0, 0, -2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));  //Original
	projMatrix = glm::perspective(70.0f, aspectRatio, 0.5f, 2000.0f);
	gShaderGA = 0;
	onExitCleanUp = false;
}
Render::~Render()
{
	if (onExitCleanUp)
	{
		delete[] blitQuads;

		delete[] spotLights;
		delete gBuffer;

		delete gaShader;
		delete lShaderObj;

		glDeleteShader(gShaderGA);
		glDeleteShader(lShader);

		onExitCleanUp = false;
	}
}

void Render::init(int GASIZE, unsigned int width, unsigned int height)
{
	gaShader = new GAShader(&gShaderGA);
	loadTextures();

	nrSpotLights = 2;
	lShaderObj = new LightShader(&lShader);
	spotLights = new SpotLight[nrSpotLights];

	//GBuffer
	gBuffer = new GBuffer();
	gBuffer->Init(width, height);

	//Screen Quads
	blitQuads = new BlitQuad[6];
	blitQuads[0].Init(&lShader, vec2(-1, -1), vec2(-0.6, -0.6));
	blitQuads[1].Init(&lShader, vec2(-0.6, -1), vec2(-0.2, -0.6));
	blitQuads[2].Init(&lShader, vec2(-0.2, -1), vec2(0.2, -0.6));
	blitQuads[3].Init(&lShader, vec2(0.2, -1), vec2(0.6, -0.6));
	blitQuads[4].Init(&lShader, vec2(0.6, -1), vec2(1, -0.6));
	blitQuads[5].Init(&lShader, vec2(-1, -1), vec2(1, 1));

	//Light
	spotLights[0].Color = vec3(1.0f, 1.0f, 1.0f);
	spotLights[0].Position = vec3(-50, 300.0f, -50);
	spotLights[0].Direction = normalize(vec3(128.0f, -80.0f, 128.0f) - vec3(-50, 300.0f, -50));
	spotLights[0].DiffuseIntensity = 1.0f;
	spotLights[0].AmbientIntensity = 0.0f;
	spotLights[0].Cutoff = 0.40f;

	spotLights[1].Color = vec3(1.0f, 1.0f, 1.0f);
	spotLights[1].Position = vec3(-50, 300.0f, -50);
	spotLights[1].Direction = normalize(vec3(128.0f, -80.0f, 128.0f) - vec3(-50, 300.0f, -50));
	spotLights[1].DiffuseIntensity = 0.40f;
	spotLights[1].AmbientIntensity = 0.10f;
	spotLights[1].Cutoff = 0.01f;

	onExitCleanUp = true;
}

void Render::loadTextures() 
{
	createTexture("GridImage.png");
	createTexture("TestAnimation/testtexture.png");
	createTexture("blacktest.png");
	createTexture("testvic.png");
	createTexture("floor.png");
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

void Render::GeometryPassInit() //Bind gBuffer for object and ground shader.
{
	glUseProgram(gShaderGA);
	gBuffer->BindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Render::render(std::vector<GObject*> renderObjects)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glUseProgram(gShaderGA);
	glProgramUniformMatrix4fv(gShaderGA, gaShader->ViewMatrix, 1, false, &viewMatrix[0][0]);
	glProgramUniformMatrix4fv(gShaderGA, gaShader->ProjectionMatrix, 1, false, &projMatrix[0][0]);
	//glProgramUniformMatrix4fv(gShaderGA, gaShader->worldMatrix, 1, false, &worldMatrixMap[0][0]);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glActiveTexture(GL_TEXTURE0);

	//Draw Player
	GLuint currentTexture = renderObjects[0]->getTexture();
	glBindTexture(GL_TEXTURE_2D, renderObjects[0]->getTexture());
	glProgramUniform1i(gShaderGA, gaShader->mapSampler, currentTexture);

	for( int i = 0; i < (int)renderObjects.size(); i++ ) {
		if( currentTexture != renderObjects[i]->getTexture() ) {
			glBindTexture(GL_TEXTURE_2D, renderObjects[i]->getTexture());
			 currentTexture = renderObjects[i]->getTexture();
		}
		renderObjects[i]->render(gaShader->worldMatrix, *gaShader->gShaderProgram);
	}


	GLenum error = glGetError();
	if(error != GL_NO_ERROR)
	{
		printf("Error");
	}
}

void Render::lightPass()
{
	glUseProgram(lShader);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gBuffer->BindForReading();

	glProgramUniform1i(lShader, lShaderObj->Position, 0);
	glProgramUniform1i(lShader, lShaderObj->Diffuse, 1);
	glProgramUniform1i(lShader, lShaderObj->Normal, 2);
	glProgramUniform1i(lShader, lShaderObj->UVcord, 3);
	glProgramUniform1i(lShader, lShaderObj->Depth, 4);

	//Camera position.
	glProgramUniform3fv(lShader, lShaderObj->eyepos, 1, &(glm::vec3(256 / 2, 200.0f, 30))[0]);
	glProgramUniform1i(lShader, lShaderObj->NumSpotLights, nrSpotLights);

	//Create and send in shadow maps view and project matrix 
	mat4 cameraview = glm::lookAt(spotLights[0].Position, spotLights[0].Position + spotLights[0].Direction, vec3(0, 1, 0));
	glProgramUniformMatrix4fv(lShader, lShaderObj->ProjectionMatrixSM, 1, false, &cameraview[0][0]); //
	glProgramUniformMatrix4fv(lShader, lShaderObj->ViewMatrixSM, 1, false, &projMatrix[0][0]);

	//Bind lights uniform buffer.
	glBindBuffer(GL_UNIFORM_BUFFER, lShaderObj->lightBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotLight) * nrSpotLights, spotLights, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, lShaderObj->bindingPoint, lShaderObj->lightBuffer);
	//------

	blitQuads[5].BindVertData();
	glProgramUniform1i(lShader, lShaderObj->Use, 5);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	for(int n = 0; n < 5; n++)
	{
		blitQuads[n].BindVertData();
		glProgramUniform1i(lShader, lShaderObj->Use, n);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	GLenum error = glGetError();
	if(error != GL_NO_ERROR)
		printf("Error");
}

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

glm::mat4 Render::getViewMatrix() const 
{
	return viewMatrix;
}
glm::mat4 Render::getProjectionMatrix() const
{
	return projMatrix;
}
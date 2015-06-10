#include <fstream>
#include <cstdlib>
#include "Render.h"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

//glm::mat4 worldMatrixMap = glm::mat4(
//	1.0f, 0.0f, 0.0f, 0.0f,
//	0.0f, 1.0f, 0.0f, 0.0f,
//	0.0f, 0.0f, 1.0f, 0.0f,
//	0.0f, 0.0f, 0.0f, 1.0f);

void Render::createBlood(float pX, float pZ)
{
	particles->createPaticleData(pX, pZ);
	nrOfBlood++;
}
void Render::removeBlood(int index)
{
	particles->removeParticleData();
	nrOfBlood--;
}


Render::Render()
{
	gShaderGA = 0;
	onExitCleanUp = false;
}
Render::Render(int GASIZE, float aspectRatio)
{
	viewMatrix = glm::lookAt(glm::vec3(455.0 / 2, GASIZE / 2, GASIZE / 2), glm::vec3(455.0 / 2, 0, GASIZE / 2), glm::vec3(0, 0, 1));
	projMatrix = glm::perspective(90.0f, 1.777778f, 0.5f, 1000.0f);
	gShaderGA = 0;
	onExitCleanUp = false;
}
Render::~Render()
{
	if (onExitCleanUp)
	{
		delete blitQuads;

		delete[] spotLights;
		delete gBuffer;

		delete gaShader;
		delete lShaderObj;

		delete shaderParticle;
		delete shaderCompute;

		delete shaderSMap;
		delete shadowMap;

		delete particles;

		glDeleteShader(gShaderGA);
		glDeleteShader(lShader);

		glDeleteShader(gShaderProgramParticle);
		glDeleteShader(gShaderProgramCompute);

		glDeleteShader(gShaderProgramSMap);

		onExitCleanUp = false;
	}
}

void Render::init(int GASIZE, unsigned int width, unsigned int height)
{
	gaShader = new GAShader(&gShaderGA);
	shaderSMap = new ShaderSMap(&gShaderProgramSMap);

	shadowMap = new ShadowMapFBO();
	shadowMap->Init(width, height);

	nrSpotLights = 1;
	nrSpotLightsShadow = 1;
	lShaderObj = new LightShader(&lShader);
	spotLights = new SpotLight[nrSpotLights];

	//GBuffer
	gBuffer = new GBuffer();
	gBuffer->Init(width, height);

	shaderParticle = new ShaderParticle(&gShaderProgramParticle);
	shaderCompute = new ShaderCompute(&gShaderProgramCompute);

	//Screen Quads
	blitQuads = new BlitQuad(&lShader, vec2(-1, -1), vec2(1, 1));

	particles = new Particles();
	//particles->createPaticleData(456 / 2, 256 / 2);
	
	//Light
	spotLights[0].Color = vec3(1.0f, 1.0f, 1.0f);
	spotLights[0].Position = vec3(-90.0f, 150.0f, 58.0f);
	spotLights[0].Direction = normalize(vec3(200.0f, 20.0f, 90.0f));
	spotLights[0].DiffuseIntensity = 1.5f;
	spotLights[0].AmbientIntensity = 0.4f;
	spotLights[0].Cutoff = 0.001f;

	onExitCleanUp = true;

	nrOfBlood = 0;
	//in = new UserInput(&viewMatrix, glm::vec3(0, 0, -4), glm::vec3(0, 0, 3), glm::vec3(0, 1, 0));
}

void Render::loadTextures() 
{
	createTexture("TestAnimation/testtexture.png");
	createTexture("blocks.png");
	createTexture("enemyobj/wolflowpoly.png");
	createTexture("blacktest.png");
	createTexture("hus/husimitten.png");
	createTexture("eldpelare/eld.png");
	createTexture("testblod.png");
}

GLuint Render::createTexture( std::string fileName ) 
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
	return texture;
}

void Render::GeometryPassInit() //Bind gBuffer for object and ground shader.
{
	glUseProgram(gShaderGA);
	gBuffer->BindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Render::renderGround(std::vector<GObject*> renderObjects)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glUseProgram(gShaderGA);
	glProgramUniformMatrix4fv(gShaderGA, gaShader->ViewMatrix, 1, false, &viewMatrix[0][0]);
	glProgramUniformMatrix4fv(gShaderGA, gaShader->ProjectionMatrix, 1, false, &projMatrix[0][0]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);

	GLuint currentTexture = renderObjects[0]->getTexture();
	glBindTexture(GL_TEXTURE_2D, renderObjects[0]->getTexture());
	glProgramUniform1i(gShaderGA, gaShader->mapSampler, currentTexture);

	renderObjects[0]->render(gaShader->worldMatrix, *gaShader->gShaderProgram);

	GLenum error = glGetError();
	if(error != GL_NO_ERROR)
	{
		printf("Error");
	}
}

void Render::render(std::vector<GObject*> renderObjects)
{
	glUseProgram(gShaderGA);
	glProgramUniformMatrix4fv(gShaderGA, gaShader->ViewMatrix, 1, false, &viewMatrix[0][0]);
	glProgramUniformMatrix4fv(gShaderGA, gaShader->ProjectionMatrix, 1, false, &projMatrix[0][0]);
	//glProgramUniformMatrix4fv(gShaderGA, gaShader->worldMatrix, 1, false, &worldMatrixMap[0][0]);
	
	//Draw Player
	GLuint currentTexture = renderObjects[0]->getTexture();
	glBindTexture(GL_TEXTURE_2D, renderObjects[0]->getTexture());
	glProgramUniform1i(gShaderGA, gaShader->mapSampler, currentTexture);

	for( int i = 1; i < (int)renderObjects.size(); i++ ) {
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


void Render::particlePass()
{
	//compute shader
	glUseProgram(gShaderProgramCompute);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particles->paticleVBO);
	glDispatchCompute(nrOfBlood, 1, 1);
	
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		printf("Error");

	//Shaders for the particle system
	glUseProgram(gShaderProgramParticle);

	glProgramUniformMatrix4fv(gShaderProgramParticle, shaderParticle->ViewMatrix, 1, false, &viewMatrix[0][0]);
	glProgramUniformMatrix4fv(gShaderProgramParticle, shaderParticle->ProjectionMatrix, 1, false, &projMatrix[0][0]);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particles->paticleVBO);

	glDrawArrays(GL_POINTS, 0, particles->sizeOfPA);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	for (int i = 0; i < nrOfBlood; i++)
	{
		particles->particlesLife[i] -= 1;

		if (particles->particlesLife[i] <= 0)
 			removeBlood(i);
	}

	error = glGetError();
	if (error != GL_NO_ERROR)
		printf("Error");
}

void Render::shadowMapPassInit()
{
	glUseProgram(gShaderProgramSMap);
	shadowMap->BindForWriting(); //clears
	glViewport(0, 0, 1280 * 10, 720 * 10);
}

void Render::shadowMapPass(std::vector<GObject*> renderObjects)
{
	viewMatrix = glm::lookAt(spotLights[0].Position, spotLights[0].Position + spotLights[0].Direction, vec3(0, 1, 0));
	glProgramUniformMatrix4fv(gShaderProgramSMap, shaderSMap->view, 1, false, &viewMatrix[0][0]);

	glProgramUniformMatrix4fv(gShaderProgramSMap, shaderSMap->proj, 1, false, &projMatrix[0][0]);
	
	for (int i = 0; i < renderObjects.size(); i++)
	{
		glProgramUniformMatrix4fv(gShaderProgramSMap, shaderSMap->model, 1, false, renderObjects[i]->returnWorldPosMat());
		renderObjects[i]->render(gaShader->worldMatrix, gShaderProgramSMap);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	viewMatrix = glm::lookAt(glm::vec3(455.0 / 2, 256 / 2, 256 / 2), glm::vec3(455.0 / 2, 0, 256 / 2), glm::vec3(0, 0, 1));	//Return camera to its pos
}

void Render::lightPass()
{
	glViewport(0, 0, 1280, 720);
	glUseProgram(lShader);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gBuffer->BindForReading();

	glProgramUniform1i(lShader, lShaderObj->Position, 0);
	glProgramUniform1i(lShader, lShaderObj->Diffuse, 1);
	glProgramUniform1i(lShader, lShaderObj->Normal, 2);
	glProgramUniform1i(lShader, lShaderObj->Depth, 4);
	glProgramUniform1i(lShader, lShaderObj->ShadowMaps, 5);

	//Camera position.
	glProgramUniform3fv(lShader, lShaderObj->eyepos, 1, &(glm::vec3(256 / 2, 200.0f, 30))[0]);
	glProgramUniform1i(lShader, lShaderObj->NumSpotLights, 1);
	glProgramUniform1i(lShader, lShaderObj->NumSpotLightsShadow, 1);
	
	shadowMap->BindForReading(5);
	//Create and send in shadow maps view and project matrix 
	mat4 cameraview = glm::lookAt(spotLights[0].Position, spotLights[0].Position + spotLights[0].Direction, vec3(0, 1, 0));
	glProgramUniformMatrix4fv(lShader, lShaderObj->ProjectionMatrixSM, 1, false, &cameraview[0][0]); 
	glProgramUniformMatrix4fv(lShader, lShaderObj->ViewMatrixSM, 1, false, &projMatrix[0][0]);


	//Bind lights uniform buffer.
	glBindBuffer(GL_UNIFORM_BUFFER, lShaderObj->lightBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotLight) * nrSpotLights, spotLights, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, lShaderObj->bindingPoint, lShaderObj->lightBuffer);
	//------

	blitQuads->BindVertData();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	GLenum error = glGetError();
	if(error != GL_NO_ERROR)
		printf("Error");
}

GLuint Render::getTexture(int index) const
{
	return textures[index];
}

GLuint Render::getTextureSize() const
{
	return textures.size();
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
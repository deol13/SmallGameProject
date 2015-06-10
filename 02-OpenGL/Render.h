#ifndef RENDER_H
#define RENDER_H

#include "Vertex.h"
#include <vector>
//#include "GameArea.h"
#include "GAShader.h"
#include <gl/glew.h>
#include <gl/GL.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "stb_image.h"
#include "GObject.h"

#include "GuiManager.h"

#include "LightShader.h"
#include "gbuffer.h"
#include "BlitQuad.h"
#include "ShaderSMap.h"
#include "ShadowMapFBO.h"
#include "Particles.h"
#include "ShaderCompute.h"
#include "ShaderParticle.h"

class Render
{
	public:
		Render();
		Render(int GASIZE, float aspectRatio);
		~Render();

		void GeometryPassInit();
		void renderGround(std::vector<GObject*> renderObjects);
		void particlePass();
		void render(std::vector<GObject*> renderObjects);
		void shadowMapPassInit();
		void shadowMapPass(std::vector<GObject*> renderObjects);
		void lightPass();
		void init(int GASIZE, unsigned int width, unsigned int height);

		GLuint gGABuffer = 0;
		GLuint gGAAttribute = 0;

		//Textures
		void loadTextures();
		GLuint createTexture( std::string fileName );

		GLuint getTexture(int index) const;
		GLuint getTextureSize() const;
		GLuint getGAShader();
		GLint getWorldMatixLoc();

		glm::mat4 getViewMatrix() const;
		glm::mat4 getProjectionMatrix() const;

		bool onExitCleanUp;
		int nrSpotLightsShadow;

		//Particles
		void createBlood(float pX, float pZ);
		void removeBlood(int index);
		int nrOfBlood;

	private:
		GAShader* gaShader;
		GLuint gShaderGA = 0;

		//Light Pass
		LightShader* lShaderObj;
		GLuint lShader = 0;

		GLuint gShaderProgramSMap = 0;
		ShaderSMap* shaderSMap;

		int nrSpotLights = 1;
		SpotLight* spotLights;

		ShadowMapFBO* shadowMap;

		//Deferred
		GBuffer* gBuffer;
		BlitQuad* blitQuads;

		GLuint gShaderProgramParticle = 0;
		ShaderParticle* shaderParticle;

		//compute shader for particle program
		GLuint gShaderProgramCompute = 0;
		ShaderCompute* shaderCompute;

		GLint ViewMatrix;
		GLint ProjectionMatrix;

		glm::mat4 viewMatrix;
		glm::mat4 projMatrix;

		GObject testObj;
		std::vector<GLuint> textures;

		Particles* particles;
};

#endif
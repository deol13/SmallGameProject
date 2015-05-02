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
#include "EnemyHandler.h"

#include "LightShader.h"
#include "gbuffer.h"
#include "BlitQuad.h"

class Render
{
	public:
		Render();
		Render(int GASIZE);
		~Render();

		void GeometryPassInit();
		void render(GuiManager* gui, std::vector<GObject*> renderObjects);
		void lightPass();
		void init(int GASIZE, unsigned int width, unsigned int height);

		GLuint gGABuffer = 0;
		GLuint gGAAttribute = 0;

		//Textures
		void loadTextures();
		void createTexture( std::string fileName );

		GLuint getTexture(int index) const;
		GLuint getGAShader();
		GLint Render::getWorldMatixLoc();

		glm::mat4 getViewMatrix() const;
		glm::mat4 getProjectionMatrix() const;

	private:
		GAShader* gaShader;
		GLuint gShaderGA = 0;

		//Light Pass
		LightShader* lShaderObj;
		GLuint lShader = 0;

		//GameArea* ga;

		int nrSpotLights = 1;
		SpotLight* spotLights;

		//Deferred
		GBuffer* gBuffer;
		BlitQuad* blitQuads;

		GLint ViewMatrix;
		GLint ProjectionMatrix;

		glm::mat4 viewMatrix;
		glm::mat4 projMatrix;

		GObject testObj;
		std::vector<GLuint> textures;

		EnemyHandler* eHandler;

};

#endif
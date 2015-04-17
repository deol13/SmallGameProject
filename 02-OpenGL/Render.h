#ifndef RENDER_H
#define RENDER_H

#include "Vertex.h"
#include <vector>
#include "GameArea.h"
#include "GAShader.h"
#include <gl/glew.h>
#include <gl/GL.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "stb_image.h"
#include "GObject.h"

#include "GuiManager.h"
#include "enemyHandler.h"

class Render
{
	public:
		Render();
		Render(int GASIZE);
		~Render();

		void render(GuiManager* gui, std::vector<GObject*> renderObjects);
		void init(int GASIZE);

		GLuint gGABuffer = 0;
		GLuint gGAAttribute = 0;

		//Textures
		void loadTextures();
		void createTexture( std::string fileName );

		GLuint getTexture(int index) const;
		GLuint getGAShader();
		GLint Render::getWorldMatixLoc();

	private:
		GAShader* gaShader;
		GLuint gShaderGA = 0;
		GameArea* ga;

		GLint ViewMatrix;
		GLint ProjectionMatrix;

		glm::mat4 viewMatrix;
		glm::mat4 projMatrix;

		GObject testObj;
		std::vector<GLuint> textures;

		enemyHandler* eHandler;

};

#endif
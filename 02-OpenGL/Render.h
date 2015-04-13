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

class Render
{
	public:
		Render();
		Render(int GASIZE);
		~Render();

		void render();
		void init(int GASIZE);

		GLuint gGABuffer = 0;
		GLuint gGAAttribute = 0;

	private:
		GAShader* gaShader;
		GLuint gShaderGA = 0;
		GameArea* ga;

		GLint ViewMatrix;
		GLint ProjectionMatrix;

		glm::mat4 viewMatrix;// = glm::lookAt(glm::vec3(256 / 2, 200.0f, 0), glm::vec3(256 / 2, 0, 256 / 2), glm::vec3(0, 1, 0));
		glm::mat4 projMatrix;// = glm::perspective(70.0f, 640.f / 480.0f, 0.5f, 2000.0f);

};

#endif
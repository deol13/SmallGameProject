#ifndef GAMEAREA_H
#define GAMEAREA_H

#include "Vertex.h"
#include <vector>
#include <gl/glew.h>
#include <gl/GL.h>


class GameArea
{
	public:
		GameArea();
		~GameArea();

		//GA
		void createGA(int GASIZE);
		void createIBO();

		void loadImage();
		void createTexture(GLubyte* gImage);

		//Buffer
		void GABuffers();

		//Get
		int getIBOCount();
		int getWidth();
		int getHeight();

		//Buffers
		GLuint gGABuffer = 0;
		GLuint gGAAttribute = 0;
		GLuint ssbo = 0;

		GLuint gIndexBuffer = 0;

		GLuint imageTex;

	private:
		//Grid
		Vertex* GAMap;
		int GAwidth;
		int GAheight;
		int GASize;

		int imageHeight;
		int imageWidth;
		

		//IBO
		int IBOCounter;

};

#endif
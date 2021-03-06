#ifndef GOBJECT_H
#define GOBJECT_H

#include <gl/glew.h>
#include <gl/GL.h>

#include "Vertex.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

class GObject
{
	private:
		//Texture
		GLuint gTexture;

		//Buffers
		GLuint gBuffer;
		GLuint gAttribute;
		GLuint IndexBufferId;

		//Vertex
		std::vector<std::vector<Vertex>> vert;
	//	std::vector<Vertex> vert2;
		std::vector<Vertex> currentVert;
		int nrOfVertices;

		float animationState;

		std::vector<GLushort> indices;

		//Matrix
		glm::mat4 scaleMatrix = glm::mat4(
			);
		glm::mat4 rotationMatrix = glm::mat4(
			);
		glm::mat4 translationMatrix = glm::mat4(
			);
		int drawMode;

	public:
		GObject();
		GObject(std::vector<Vertex> vertices, int drawMode, GLuint gTexture);
		GObject(std::string fileName, int drawMode, GLuint texture);					
		GObject(std::string* fileNames, int nrOfKeyFrames, GLuint texture);					//Loads multiple keyframes for animated objects
		~GObject();

		void scale(float x, float y, float z); //set scale matrix
		void rotate(float x, float y, float z); //set rotation matrix
		void setRotation(float x, float y, float z);
		void translate(float x, float y, float z); //set translation matrix
		

		void loadObjectFile(std::string fileName, int keyFrame = 0);
		void bindBuffers();

		void render(GLint uniLocation, GLuint shaderProgram); //Draw

		void init();

		void animate(int stopState);

		void setAnimationState(float state);
		void setTexture(GLuint gTexture);
		GLuint getTexture();
		void setVertices(std::vector<Vertex> vertices);
		std::vector<Vertex> getVertices();
		
		int getNrOfVertices();
		int getNrOfKeyFrames();
		GLfloat* returnWorldPosMat();
};

#endif
#ifndef ENEMYHANDLER_H
#define ENEMYHANDLER_H
#include "enemy.h"


class enemyHandler
{
	private:
		enemy* wave;
		int wavesize;


		GLuint shaderProgram;
		GLint uniLocation;

	public:	
		enemyHandler();
		enemyHandler(GLuint shaderProgram);
		~enemyHandler();

		int getWaveSize();

		void createWave(int melee, GObject* meleeObj, int ranged, GObject* rangedObj, int tank, GObject* tankObj);
		
		void clearWave();

		void makeMove(float playerX, float playerZ);	//update

		void setUniLoc(GLint uniLocation);
		void setShaderPro(GLuint ShaderProgram);



};
#endif
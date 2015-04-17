#ifndef ENEMYHANDLER_H
#define ENEMYHANDLER_H
#include "enemy.h"
#include "GObject.h"

class enemyHandler
{
	private:
		enemy* wave;
		int wavesize;
		Player* player;

		GLuint shaderProgram;
		GLint uniLocation;

	public:	
		enemyHandler();
		enemyHandler(Player* player, GLuint shaderProgram);
		~enemyHandler();

		int getWaveSize();

		void createWave(int melee, GObject* meleeObj, int ranged, GObject* rangedObj, int tank, GObject* tankObj);
		
		void clearWave();

		void makeMove();	//update

		void setUniLoc(GLint uniLocation);
		void setShaderPro(GLuint ShaderProgram);



};
#endif
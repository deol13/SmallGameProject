#ifndef EnemyHANDLER_H
#define EnemyHANDLER_H
#include "Enemy.h"


class EnemyHandler
{
	private:
		Enemy* wave;
		int wavesize;


		GLuint shaderProgram;
		GLint uniLocation;

	public:	
		EnemyHandler();
		EnemyHandler(GLuint shaderProgram);
		~EnemyHandler();

		int getWaveSize();

		void createWave(int melee, GObject* meleeObj, int ranged, GObject* rangedObj, int tank, GObject* tankObj);
		
		void clearWave();

		void makeMove(float playerX, float playerZ);	//update

		void setUniLoc(GLint uniLocation);
		void setShaderPro(GLuint ShaderProgram);



};
#endif
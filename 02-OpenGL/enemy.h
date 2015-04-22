#ifndef Enemy_H
#define Enemy_H
#include "GObject.h"

const int MELEE = 1;
const int RANGED = 2;
const int TANK = 3;

class Enemy
{
	private:
		float x;
	//	float posY = 1;
		float z;
		float moveSpeed;
		int health;
		int type;
		GObject* loadObj;

	public:
		Enemy();
		Enemy(int type, float x, float z);
		~Enemy();

		void setEnemy(int type);
		void act(float playerX, float playerZ); //spelarens objekt eller plats

};
#endif
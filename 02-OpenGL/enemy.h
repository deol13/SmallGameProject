#ifndef ENEMY_H
#define ENEMY_H
#include "GObject.h"

const int MELEE = 1;
const int RANGED = 2;
const int TANK = 3;

class enemy
{
	private:
		float posX;
		float posY = 1;
		float posZ;
		float moveSpeed;

	public:
		int health;
		int type;
		GObject* loadObj;

		enemy();
		~enemy();

		void setEnemy(int type);
		void act(float playerX, float playerZ); //spelarens objekt eller plats

};
#endif
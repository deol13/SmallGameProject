#ifndef Enemy_H
#define Enemy_H
#include "GObject.h"
#include "BoundingRect.h"

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
		BoundingRect collisionRect;

	public:
		Enemy();
		Enemy(int type, float x, float z, GLuint texture);
		~Enemy();

		void setEnemy(int type);
		void act(float playerX, float playerZ); //spelarens objekt eller plats
		GObject* getGObject() const;
		BoundingRect getBounds() const;

};
#endif
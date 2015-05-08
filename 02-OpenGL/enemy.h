#ifndef Enemy_H
#define Enemy_H
#include "GObject.h"
#include "BoundingRect.h"

const int MELEE = 1;
const int RANGED = 2;
const int TANK = 3;

const int MELEERANGE = 8;
const int RANGEDRANGE = 16;
const int TANKRANGE = 12;

const int POTENTIALRANGE = 4;

using namespace std;

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

		int firstLastX, firstLastZ, secondLastX, secondLastZ, thirdLastX, thirdLastZ;
		void setPheromone(int x, int z, int** board);
		void initPhero();

	public:
		Enemy();
		Enemy(int type, float x, float z, GLuint texture);
		~Enemy();

		void setEnemy(int type);
		
		GObject* getGObject() const;
		float getX() const;
		float getZ() const;
		int getHealth()const;
		BoundingRect getBounds() const;
		bool takeDamage(const int dmg);					//returns true if still alive

		void attack();
		void act(float playerX, float playerZ, int** board);

};
#endif
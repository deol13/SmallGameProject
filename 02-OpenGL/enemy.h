#ifndef Enemy_H
#define Enemy_H
#include "GObject.h"
#include "BoundingPolygon.h"
#include "Audio.h"

const int MELEE = 1;
const int RANGED = 2;
const int ANIMAL = 3;

const int MELEERANGE = 8;
const int RANGEDRANGE = 16;
const int ANIMALRANGE = 12;

const int potentialRange = 8;

class Enemy
{
	private:
		float x;
	//	float posY = 1;
		float z;
		float moveSpeed;
		int health;
		int type;
		int attackRange;
		GObject* loadObj;
		BoundingPolygon collisionRect;
		float neighbourPos[8];					//index goes NW, N, NE, W, E, SW, S, SE

	public:
		Enemy();
		Enemy(int type, float x, float z, GLuint texture, string objectFile, int waveNr);
		~Enemy();

		void setEnemy(int type);
		
		GObject* getGObject() const;
		float getX()const;
		float getZ()const;
		int getHealth()const;
		int getRange()const;
		BoundingPolygon getBounds() const;
		bool takeDamage(const int dmg);					//returns true if still alive

		int attack();
		void move();

		/*adds base power at distance 1 from origin and degrades with radius*/
		void setPotential(int origX, int origZ, int basePower);
		void clearPotential(const int staticBoard[256][256]);

};
#endif
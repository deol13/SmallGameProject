#ifndef Enemy_H
#define Enemy_H
#include "GObject.h"
#include "BoundingPolygon.h"
#include "Audio.h"

const int MELEE = 1;
const int RANGED = 2;
const int ANIMAL = 3;
const int FIRSTBOSS = 4;
const int SECONDBOSS = 5;
const int FINALBOSS = 6;

const int MELEERANGE = 5;
const int RANGEDRANGE = 16;
const int ANIMALRANGE = 5;

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
		int attackTime;
		GObject* loadObj;
		BoundingPolygon collisionRect;
		float neighbourPos[8];					//index goes NW, N, NE, W, E, SW, S, SE
		bool charging;
		int chargeTimer;
		int idleTimer;
		int dirIndex;
		int dirTimer;

	public:
		Enemy();
		Enemy(int type, float x, float z, GLuint texture, string objectFile, int waveNr);
		Enemy(int type, float x, float z, GLuint texture, string* objectFiles, int nrOfKeyFrames, int waveNr, int idle);
		~Enemy();

		void setEnemy(int type);
		
		GObject* getGObject() const;
		float getX()const;
		float getZ()const;
		int getHealth()const;
		int getRange()const;
		int getType()const;
		BoundingPolygon getBounds() const;
		bool takeDamage(const int dmg, const float playerX, const float playerZ);					//returns true if still alive
		bool isAlive()const;
		bool isCharging()const;
		int getChargeTimer()const;
		bool isIdle()const;
		bool isTangible()const;
		void changeIdle();
		void updateCharge();

		int attack();
		void move();

		/*adds base power at distance 1 from origin and degrades with radius*/
		void setPotential(int origX, int origZ, int basePower);
		void clearPotential(const float staticBoard[455][256]);
		void clearPotential();

};
#endif
#ifndef PLAYER_H
#define PLAYER_H

#include "BoundingPolygon.h"
#include "GObject.h"
#include <ctime>

const int SWORD = 1;
const int SPEAR = 2;

class Player 
{
private:
	int health;
	int maxHealth;
	int armour;
	int gold;
	
	int weapon;
	int spearUpgrade;
	int swordUpgrade;

	clock_t start;

	float x, z;
	float angle;
	float moveSpeed;
	GObject** loadObj;
	bool movement[4];
	BoundingPolygon collisionRect;

public:
	enum Direction { UP, DOWN, LEFT, RIGHT, STILL };
	Player();
	Player(GLuint texture, float x, float y, int health, int armour);
	~Player();
	void setMovement(int dir, bool isMoving);
	bool takeDamage(const int dmg);
	GObject** getGObjects() const;
	void update();

	void setMaxHealth(const int health);
	void setHealth(const int health);
	void setArmour(const int armour);
	
	void setWeapon(const int weapon);
	void setWeaponUpgrade(const int weapon, const int weaponLevel);
	void setGold(const int gold);

	int getDamageDealt();
	int getWeapon();
	int getHealth();
	float getX() const;
	float getZ() const;
	float getAngle() const;
	BoundingPolygon getBounds() const;
	float getMoveSpeed() const;
	int getGold() const;
};
#endif
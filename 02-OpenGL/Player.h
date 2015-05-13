#ifndef PLAYER_H
#define PLAYER_H

#include "BoundingPolygon.h"
#include "GObject.h"

class Player 
{
private:
	int health;
	int maxHealth;
	int armour;
	int gold;
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
	void attack();
	void setMovement(int dir, bool isMoving);
	bool takeDamage(const int dmg);
	GObject** getGObjects() const;
	void update();

	void setGold(const int gold);
	void setMaxHealth(const int health);
	void setHealth(const int health);
	void setArmour(const int armour);

	float getX() const;
	float getZ() const;
	float getAngle() const;
	BoundingPolygon getBounds() const;
	float getMoveSpeed() const;
};
#endif
#ifndef PLAYER_H
#define PLAYER_H

#include "BoundingPolygon.h"
#include "GObject.h"

class Player 
{
private:
	int health;
	int gold;
	float x, z;
	float angle;
	float moveSpeed;
	GObject* loadObj;
	bool movement[4];
	BoundingPolygon collisionRect;

public:
	enum Direction { UP, DOWN, LEFT, RIGHT, STILL };
	Player();
	Player(GLuint texture, float x, float y);
	~Player();
	void attack();
	void setMovement(int dir, bool isMoving);
	bool takeDamage(const int dmg);
	GObject* getGObject() const;
	void update();

	float getX() const;
	float getZ() const;
	float getAngle() const;
	BoundingPolygon getBounds() const;
	float getMoveSpeed() const;
};
#endif
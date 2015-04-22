#ifndef PLAYER_H
#define PLAYER_H

#include "GObject.h"

class Player 
{
private:
	int health;
	int gold;
	float x, z;
	float moveSpeed;
	GObject* loadObj;
	bool movement[4];

public:
	enum Direction { UP, DOWN, LEFT, RIGHT };
	Player();
	Player(GLuint texture, float x, float y);
	~Player();
	void attack();
	void setMovement(int dir, bool isMoving);
	void takeDamage(int dmg);
	GObject* getGObject() const;
	void update();

	float getX();
	float getZ();
};
#endif
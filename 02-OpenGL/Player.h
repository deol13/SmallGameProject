#ifndef PLAYER_H
#define PLAYER_H

#include "BoundingPolygon.h"
#include "GObject.h"

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

	int invulTimer;
	int attackState;

	float x, z;
	glm::vec2 dirVec;		//does not reset when stopping
	glm::vec2 moveVec;		//does reset when stopping
	float moveSpeed;
	GObject** loadObj;
	//bool movement[4];
	BoundingPolygon collisionRect;

public:
	//enum Direction { UP, DOWN, LEFT, RIGHT, STILL };
	Player();
	Player(GLuint textures[3], float x, float y, int health, int armour);
	~Player();
	void setMovement(int x, int y);
	void stop(bool stopX, bool stopZ);
	int takeDamage(int dmg);
	GObject* getGObject(int index) const;
	void update(const float board[455][256]);
	void attack();

	void setMaxHealth(const int health);
	void setHealth(const int health);
	void setArmour(const int armour);
	
	void setWeapon(const int weapon);
	void setWeaponUpgrade(const int weapon, const int weaponLevel);
	void setGold(const int gold);
	void setDefault();

	int getDamageDealt();
	int getWeapon();
	int getWeaponUpgrade(int weaponChosen);
	int getMaxHealth();
	int getHealth();
	int getArmour();
	int getInvulTimer();

	float getX() const;
	float getZ() const;
	glm::vec2 getDirection() const;
	BoundingPolygon getBounds() const;
	float getMoveSpeed() const;
	int getGold() const;
};
#endif
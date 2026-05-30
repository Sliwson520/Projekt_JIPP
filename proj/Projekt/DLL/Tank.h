#pragma once

#include "Entity.h"

class ENGINE_API Tank :public Entity{
private:
	int health;
	int maxHealth;
public:
	Tank(float startX, float startY, float startSpeed, int hp);
	void update(float deltaTime) override;
	void move(int dir, float deltaTime);
	void takeDamage(int amount);
	bool isDead() const { return health <= 0; }
};

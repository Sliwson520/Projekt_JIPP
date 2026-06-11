#pragma once
#include "Entity.h"
#include "API.h"
#include <string>


class ENGINE_API Tank :public Entity{
private:
	int health;
	int maxHealth;
	int direction;

	sf::Texture texture;
	sf::Sprite sprite;

	bool isPlayer;
	float aiTimer;
	float changeDirTime;
	float shootTimer;

public:
	Tank(float startX, float startY, float startSpeed, int hp, const std::string& texturePath, bool player=false);
	class Bullet* update(float deltaTime) override;
	void draw(sf::RenderWindow& window) override;
	void move(int dir, float deltaTime);
	void takeDamage(int amount);
	int getDirection() const { return direction; }
	sf::FloatRect getBounds() const override;
	void heal(int amount);
	friend class HUD;
};

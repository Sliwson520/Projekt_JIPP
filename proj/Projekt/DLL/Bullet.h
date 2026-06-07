#ifndef BULLET_H
#define BULLET_H

#pragma once
#include "Entity.h"
#include "API.h"
#include <string>

class ENGINE_API Bullet : public Entity {
private:
	int direction;
	sf::Texture texture;
	sf::Sprite sprite;
public:
	Bullet(float startX, float startY, float startSpeed, int dir, const std::string& texturePath);

	void update(float deltaTime) override;
	void draw(sf::RenderWindow& window) override;
	sf::FloatRect getBounds() const override;
};
#endif
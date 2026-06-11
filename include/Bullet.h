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
	bool fromPlayer;
	float speed;
public:
	Bullet(float x, float y, float speed, int direction, const std::string& texturePath, bool isPlayerBullet);
	bool isFromPlayer() const { return fromPlayer; }
	Bullet* update(float deltaTime) override;
	void draw(sf::RenderWindow& window) override;
	sf::FloatRect getBounds() const override;
};
#endif
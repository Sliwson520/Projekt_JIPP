#include "Bullet.h"
#include <iostream>

Bullet::Bullet(float startX, float startY, float startSpeed, int dir, const std::string& texturePath)
	: Entity(startX, startY, startSpeed), direction(dir)
{
	if (!texture.loadFromFile(texturePath)) {
		std::cout << "Blad ladowania tekstury pocisku: " << texturePath << std::endl;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);

	if (direction == 0) sprite.setRotation(0.0f);
	else if (direction == 1) sprite.setRotation(90.0f);
	else if (direction == 2) sprite.setRotation(180.0f);
	else if (direction == 3) sprite.setRotation(270.0f);
}

void Bullet::update(float deltaTime) {
	if (direction == 0) {
		y -= speed * deltaTime;
	}
	else if (direction == 1) {
		x += speed * deltaTime;
	}
	else if (direction == 2) {
		y += speed * deltaTime;
	}
	else if (direction == 3) {
		x -= speed * deltaTime;
	}
}
void Bullet::draw(sf::RenderWindow& window) {
	if (isActive()) {
		sprite.setPosition(x, y);
		window.draw(sprite);
	}
}
sf::FloatRect Bullet::getBounds() const {
	return sprite.getGlobalBounds();
}

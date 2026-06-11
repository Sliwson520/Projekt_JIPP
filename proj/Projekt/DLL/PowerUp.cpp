#include "PowerUp.h"
#include <iostream>

PowerUp::PowerUp(float startX, float startY, const std::string& texturePath)
    : Entity(startX, startY, 0.0f)
{
    if (!texture.loadFromFile(texturePath)) {
        std::cout << "Blad ladowania tekstury PowerUp: " << texturePath << std::endl;
    }
    sprite.setTexture(texture);
    sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
    sprite.setPosition(x, y);
}

void PowerUp::update(float deltaTime) {}

void PowerUp::draw(sf::RenderWindow& window) {
    if (isActive()) {
        window.draw(sprite);
    }
}

sf::FloatRect PowerUp::getBounds() const {
    return sprite.getGlobalBounds();
}
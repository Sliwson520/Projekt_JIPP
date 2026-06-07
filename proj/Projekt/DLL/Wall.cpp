#include "Wall.h"
#include <iostream>

Wall::Wall(float startX, float startY, const std::string& texturePath)
    : Entity(startX, startY, 0.0f)
{
    if (!texture.loadFromFile(texturePath)) {
        std::cout << "Blad ladowania tekstury sciany: " << texturePath << std::endl;
    }
    sprite.setTexture(texture);

    sprite.setPosition(x, y);
}

void Wall::update(float deltaTime) {
}

void Wall::draw(sf::RenderWindow& window) {
    if (isActive()) {
        window.draw(sprite);
    }
}

sf::FloatRect Wall::getBounds() const {
    return sprite.getGlobalBounds();
}
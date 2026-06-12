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

Bullet* Wall::update(float deltaTime) {
    return nullptr;
}
void Wall::draw(sf::RenderWindow& window) {
    if (isActive()) {
        window.draw(sprite);
    }
}

sf::FloatRect Wall::getBounds() const {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    return sf::FloatRect(bounds.left + 1.0f, bounds.top + 1.0f, bounds.width - 2.0f, bounds.height - 2.0f);
}
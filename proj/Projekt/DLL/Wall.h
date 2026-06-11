#ifndef WALL_H
#define WALL_H

#pragma once
#include "Entity.h"
#include "API.h"
#include <string>

class ENGINE_API Wall : public Entity {
private:
    sf::Texture texture;
    sf::Sprite sprite;

public:
    Wall(float startX, float startY, const std::string& texturePath);

    class Bullet* update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
};

#endif
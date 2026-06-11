#ifndef POWERUP_H
#define POWERUP_H

#pragma once
#include "Entity.h"
#include "API.h"
#include <string>

class ENGINE_API PowerUp : public Entity {
protected:
    sf::Texture texture;
    sf::Sprite sprite;

public:
    PowerUp(float startX, float startY, const std::string& texturePath);

    class Bullet* update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;

    virtual void applyEffect(class Tank* player) = 0;
};

#endif
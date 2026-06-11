#include "Tank.h"
#include "Bullet.h"
#include <iostream>
#include <cstdlib>
Tank::Tank(float startX, float startY, float startSpeed, int hp, const std::string& texturePath, bool player)
    : Entity(startX, startY, startSpeed), health(hp), maxHealth(hp), direction(0), isPlayer(player), aiTimer(0.0f),changeDirTime(1.5f), shootTimer(0.0f)
{
    if (!texture.loadFromFile(texturePath)) {
        std::cout << "Blad ladowania tekstury w klasie Tank: " << texturePath << std::endl;
    }
    sprite.setTexture(texture);
    sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
}
Bullet* Tank::update(float deltaTime) {
    if (!isActive()) return nullptr;

    if (!isPlayer) {
        aiTimer += deltaTime;
        shootTimer += deltaTime;

        if (aiTimer >= changeDirTime) {
            direction = rand() % 4;
            aiTimer = 0.0f;
            changeDirTime = 0.5f + static_cast<float>(rand() % 15) / 10.0f;
        }
        move(direction, deltaTime);

        if (shootTimer >= 1.8f) {
            shootTimer = 0.0f;
            return new Bullet(x, y, 300.0f, direction, "bullet.png");
        }
    }
    return nullptr;
}

void Tank::draw(sf::RenderWindow& window) {
    sprite.setPosition(x, y);
    window.draw(sprite);
}

void Tank::move(int dir, float deltaTime ) {
    direction = dir;
    if (direction == 0) {
        y -= speed * deltaTime;
        sprite.setRotation(0.0f);
    }
    else if (direction == 1) {
        x += speed * deltaTime;
        sprite.setRotation(90.0f);
    }
    else if (direction == 2) {
        y += speed * deltaTime;
        sprite.setRotation(180.0f);
    }
    else if (direction == 3) {
        x -= speed * deltaTime;
        sprite.setRotation(270.0f);
    }
}
void Tank::takeDamage(int amount) {
    health -= amount;
    if (health < 0) health = 0;
}

sf::FloatRect Tank::getBounds() const {
    return sprite.getGlobalBounds();
}

void Tank::heal(int amount) {
    health += amount;
    if (health > maxHealth) health = maxHealth;
    std::cout << "Uleczono czolg! Aktualne HP: " << health << std::endl;
}
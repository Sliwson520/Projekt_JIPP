#include "DestructibleWall.h"

DestructibleWall::DestructibleWall(float startX, float startY, const std::string& texturePath)
    : Wall(startX, startY, texturePath), health(1)
{
}

void DestructibleWall::takeDamage(int amount) {
    health -= amount;
    if (health <= 0) {
        destroy();
    }
}
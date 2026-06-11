#ifndef DESTRUCTIBLE_WALL_H
#define DESTRUCTIBLE_WALL_H

#pragma once
#include "Wall.h"

class ENGINE_API DestructibleWall : public Wall {
private:
    int health;

public:
    DestructibleWall(float startX, float startY, const std::string& texturePath);

    void takeDamage(int amount);
};

#endif
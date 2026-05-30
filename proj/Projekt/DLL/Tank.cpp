#include "Tank.h"

Tank::Tank(float startX, float startY, float startSpeed, int hp) : Entity(startX, startY, startSpeed), health(hp), maxHealth(hp){}
void Tank::update(float deltaTime) {

}
void Tank::move(int dir, float deltaTime ) {
    direction = dir;
    if (direction == 0) y -= speed * deltaTime;
    if (direction == 1) x += speed * deltaTime;
    if (direction == 2) y += speed * deltaTime;
    if (direction == 3) x -= speed * deltaTime;
}
void Tank::takeDamage(int amount) {
    health -= amount;
    if (health < 0) health = 0;
}
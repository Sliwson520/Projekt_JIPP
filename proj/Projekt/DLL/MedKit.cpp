#include "MedKit.h"
#include "Tank.h"

MedKit::MedKit(float startX, float startY, const std::string& texturePath)
    : PowerUp(startX, startY, texturePath)
{
}

void MedKit::applyEffect(Tank* player) {
    player->heal(25);
    destroy();
}
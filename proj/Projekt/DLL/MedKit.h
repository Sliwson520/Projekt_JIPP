#ifndef MEDKIT_H
#define MEDKIT_H

#pragma once
#include "PowerUp.h"

class ENGINE_API MedKit : public PowerUp {
public:
    MedKit(float startX, float startY, const std::string& texturePath);
    void applyEffect(class Tank* player) override;
};

#endif
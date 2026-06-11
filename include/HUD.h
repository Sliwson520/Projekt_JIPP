#ifndef HUD_H
#define HUD_H

#pragma once
#include <SFML/Graphics.hpp>
#include "API.h"

class ENGINE_API HUD {
private:
    sf::Font font;
    sf::Text hpText;

public:
    HUD(const std::string& fontPath);

    void draw(sf::RenderWindow& window, class Tank* player);
};

#endif
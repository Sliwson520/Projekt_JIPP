#include "HUD.h"
#include "Tank.h"
#include <iostream>

HUD::HUD(const std::string& fontPath) {
    if (!font.loadFromFile(fontPath)) {
        std::cout << "Blad ladowania czcionki HUD: " << fontPath << std::endl;
    }

    hpText.setFont(font);
    hpText.setCharacterSize(24);
    hpText.setFillColor(sf::Color::White);
    hpText.setPosition(20.0f, 20.0f);
}

void HUD::draw(sf::RenderWindow& window, Tank* player) {
    if (player && player->isActive()) {
        std::string info = "HP Gracza: " + std::to_string(player->health) + " / " + std::to_string(player->maxHealth);
        hpText.setString(info);

        window.draw(hpText);
    }
    else {
        hpText.setString("GAME OVER - Zginales!");
        hpText.setPosition(300.0f, 280.0f);
        window.draw(hpText);
    }
}
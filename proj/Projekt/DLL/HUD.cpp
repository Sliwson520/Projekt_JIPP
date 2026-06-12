#include "HUD.h"
#include "Tank.h"
#include <iostream>

HUD::HUD(const std::string& fontPath) {
    if (!font.loadFromFile(fontPath)) {
        std::cout << "Blad ladowania czcionki HUD: " << fontPath << std::endl;
    }

    hpText.setFont(font);
    hpText.setCharacterSize(22);
    hpText.setFillColor(sf::Color::White);
    hpText.setPosition(20.0f, 20.0f);
}

void HUD::draw(sf::RenderWindow& window, Tank* player, int currentScore, int highScore) {
    if (player && player->isActive()) {
        std::string info = "HP: " + std::to_string(player->health) + " / " + std::to_string(player->maxHealth) +
            "   |   WYNIK: " + std::to_string(currentScore) +
            "   |   REKORD (BIN): " + std::to_string(highScore);
        hpText.setString(info);
        hpText.setPosition(20.0f, 20.0f);
        hpText.setFillColor(sf::Color::White);
        window.draw(hpText);
    }
    else {
        hpText.setString("GAME OVER!\n\nTwoj wynik: " + std::to_string(currentScore) + "\n\nNacisnij Enter, aby zagrac jeszcze raz."+
            "\n\nNacisnij ESC, aby wrocic do Menu.");
        hpText.setPosition(260.0f, 220.0f);
        hpText.setFillColor(sf::Color::Red);
        window.draw(hpText);
    }
}

void HUD::drawMenu(sf::RenderWindow& window, int highScore) {
    std::string menuContent = "===============================\n"
        "       SUPER CZOLGI JIPP\n"
        "===============================\n\n"
        "  [ENTER] - Rozpocznij Gre\n"
        "  [ESC]   - Wyjdz z programu\n\n\n"
        "Aktualny Rekord (Wczytany z pliku BIN):\n"
        "  >>>  " + std::to_string(highScore) + " PKT  <<<";

    hpText.setString(menuContent);
    hpText.setPosition(200.0f, 160.0f);
    hpText.setFillColor(sf::Color::Yellow);
    window.draw(hpText);
}
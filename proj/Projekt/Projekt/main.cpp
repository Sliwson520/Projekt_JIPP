#include <SFML/Graphics.hpp>
#include <iostream>
#include "Tank.h"

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 600), "Czolgi JIPP - Projekt");
	window.setFramerateLimit(60);

	Tank myTank(400.0f, 300.0f, 200.0f, 100);

	sf::RectangleShape tankSprite(sf::Vector2f(40.0f, 40.0f));
	tankSprite.setFillColor(sf::Color::Green);
	tankSprite.setOrigin(20.0f, 20.0f);

	sf::Clock clock;

	std::cout << "Gra uruchomiona! Steruj strzalkami." << std::endl;

	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		float deltaTime = clock.restart().asSeconds();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			myTank.move(0, deltaTime);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			myTank.move(1, deltaTime);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			myTank.move(2, deltaTime);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			myTank.move(3, deltaTime);
		}

		tankSprite.setPosition(myTank.getX(), myTank.getY());

		window.clear(sf::Color(30, 30, 30));
		window.draw(tankSprite);
		window.display();
	}
	return 0;
}
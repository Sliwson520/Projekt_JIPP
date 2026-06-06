#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <ctime>
#include "Tank.h"
#include "Bullet.h"
#include "Entity.h"


int main() {
	srand(static_cast<unsigned int>(time(NULL)));
	sf::RenderWindow window(sf::VideoMode(800, 600), "Czolgi JIPP - Projekt");
	window.setFramerateLimit(60);
	std::vector<Entity*> gameObjects;

	Tank* player = new Tank(200.0f, 200.0f, 150.0f, 100, "tank.png",true);
	gameObjects.push_back(player);

	gameObjects.push_back(new Tank(500.0f, 150.0f, 100.0f, 50, "tank.png"));
	gameObjects.push_back(new Tank(600.0f, 400.0f, 100.0f, 50, "tank.png"));

	sf::Clock clock;
	int currentDir = 0;
	bool spacePressed = false;

	std::cout << "Gra uruchomiona! Steruj strzalkami." << std::endl;

	while (window.isOpen()) {
		float deltaTime = clock.restart().asSeconds();

		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			player->move(0, deltaTime);
			currentDir = 0;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			player->move(1, deltaTime);
			currentDir = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			player->move(2, deltaTime);
			currentDir = 2;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			player->move(3, deltaTime);
			currentDir = 3;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			if (!spacePressed) {
				Bullet* newBullet = new Bullet(player->getX(), player->getY(), 400.0f, currentDir, "bullet.png");

				gameObjects.push_back(newBullet);

				spacePressed = true;
			}
		}
		else {
			spacePressed = false;
		}

		for (Entity* obj : gameObjects) {
			obj->update(deltaTime);

			if (obj->getX() < 0 || obj->getX() > 800 || obj->getY() < 0 || obj->getY() > 600) {
				obj->destroy();
			}
		}

		for (auto it = gameObjects.begin(); it != gameObjects.end(); ) {
			if (!(*it)->isActive()) {
				delete* it;
				it = gameObjects.erase(it);
			}
			else {
				++it;
			}
		}


		window.clear(sf::Color(50, 50, 50));
		for (Entity* obj : gameObjects) {
			obj->draw(window);
		}

		window.display();
	}
		for (Entity* obj : gameObjects) {
			delete obj;
		}
		gameObjects.clear();
		

	return 0;
}
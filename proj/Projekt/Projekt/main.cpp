#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <ctime>
#include "Tank.h"
#include "Bullet.h"
#include "Entity.h"
#include "Wall.h"
#include "DestructibleWall.h"
#include "MedKit.h"
#include "HUD.h"

int main() {
	srand(static_cast<unsigned int>(time(NULL)));
	sf::RenderWindow window(sf::VideoMode(800, 600), "Czolgi JIPP - Projekt");
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);
	std::vector<Entity*> gameObjects;

	Tank* player = new Tank(200.0f, 200.0f, 150.0f, 100, "tank.png",true);
	gameObjects.push_back(player);

	gameObjects.push_back(new Tank(500.0f, 150.0f, 100.0f, 50, "tank.png"));
	gameObjects.push_back(new Tank(600.0f, 400.0f, 100.0f, 50, "tank.png"));

	gameObjects.push_back(new Wall(400.0f, 300.0f, "wall.png"));
	gameObjects.push_back(new Wall(450.0f, 300.0f, "wall.png"));
	gameObjects.push_back(new Wall(500.0f, 300.0f, "wall.png"));

	gameObjects.push_back(new DestructibleWall(550.0f, 300.0f, "destructible_wall.png"));

	gameObjects.push_back(new MedKit(300.0f, 100.0f, "medkit.png"));
	HUD hud("arial.ttf");


	sf::Clock clock;
	int currentDir = 0;
	bool spacePressed = false;

	std::cout << "Gra uruchomiona! Steruj strzalkami." << std::endl;

	while (window.isOpen()) {
		float deltaTime = clock.restart().asSeconds();
		if (deltaTime > 0.1f) deltaTime = 0.1f;
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

		for (Entity* objA : gameObjects) {
			Bullet* bullet = dynamic_cast<Bullet*>(objA);
			if (bullet && bullet->isActive()) {

				for (Entity* objB : gameObjects) {
					Tank* tank = dynamic_cast<Tank*>(objB);

					if (tank && tank->isActive() && tank->getBounds().intersects(bullet->getBounds())) {

						if (tank == player) {
							continue;
						}

						tank->takeDamage(25);

						bullet->destroy();

						std::cout << "Trafienie! Czolg wroga oberwal!" << std::endl;
						break;
					}
				}
			}

		}

		for (Entity* obj : gameObjects) {
			Bullet* bullet = dynamic_cast<Bullet*>(obj);
			if (bullet && bullet->isActive()) {
				for (Entity* wallObj : gameObjects) {
					Wall* wall = dynamic_cast<Wall*>(wallObj);
					if (wall && bullet->getBounds().intersects(wall->getBounds())) {
						DestructibleWall* destWall = dynamic_cast<DestructibleWall*>(wallObj);
						if (destWall) {
							destWall->takeDamage(1);
						}

						bullet->destroy();
						break;
					}
				}
			}

			Tank* tank = dynamic_cast<Tank*>(obj);
			if (tank && tank->isActive()) {
				for (Entity* wallObj : gameObjects) {
					Wall* wall = dynamic_cast<Wall*>(wallObj);
					if (wall && tank->getBounds().intersects(wall->getBounds())) {
						int dir = tank->getDirection();
						if (dir == 0) tank->move(2, deltaTime * 1.2f);
						else if (dir == 1) tank->move(3, deltaTime * 1.2f);
						else if (dir == 2) tank->move(0, deltaTime * 1.2f);
						else if (dir == 3) tank->move(1, deltaTime * 1.2f);
					}
				}
			}
			if (tank && tank == player && tank->isActive()) {
				for (Entity* powerObj : gameObjects) {
					PowerUp* power = dynamic_cast<PowerUp*>(powerObj);
					if (power && power->isActive() && tank->getBounds().intersects(power->getBounds())) {
						power->applyEffect(tank);
						break;
					}
				}
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
		hud.draw(window, player);

		window.display();

	}
		for (Entity* obj : gameObjects) {
			delete obj;
		}
		gameObjects.clear();
		

	return 0;
}
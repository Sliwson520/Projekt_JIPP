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
#include "ScoreManager.h"

enum GameState { MENU, GAME, GAMEOVER };

int main() {
	srand(static_cast<unsigned int>(time(NULL)));
	sf::RenderWindow window(sf::VideoMode(800, 600), "Czolgi JIPP - Projekt");
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);

	GameState state = MENU;
	std::vector<Entity*> gameObjects;

	int score = 0;
	int highScore = 0;

	// --- 1. OBS£UGA WYJ¥TKÓW I PLIKÓW BINARNYCH NA STARCE ---
	try {
		highScore = ScoreManager::loadHighScore("highscore.dat");
		std::cout << "Pomyslnie wczytano rekord binarny: " << highScore << std::endl;
	}
	catch (const FileException& e) {
		std::cout << "Notatka: " << e.what() << " (Uruchomienie po raz pierwszy, ustawiam rekord na 0)." << std::endl;
		highScore = 0;
	}

	Tank* player = nullptr;
	HUD hud("arial.ttf");
	sf::Clock clock;
	int currentDir = 0;
	bool spacePressed = false;
	bool scoreSaved = false;

	std::cout << "Gra uruchomiona! Steruj strzalkami." << std::endl;

	while (window.isOpen()) {
		float deltaTime = clock.restart().asSeconds();
		if (deltaTime > 0.1f) deltaTime = 0.1f;
		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// --- LOGIKA ZALE¯NA OD STANU GRY ---
		if (state == MENU) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
				// Czyszczenie starej planszy na wypadek restartu
				for (Entity* obj : gameObjects) delete obj;
				gameObjects.clear();

				score = 0;
				scoreSaved = false;

				// Inicjalizacja obiektów gry
				player = new Tank(200.0f, 200.0f, 150.0f, 100, "tank.png", true);
				gameObjects.push_back(player);

				gameObjects.push_back(new Tank(500.0f, 150.0f, 100.0f, 50, "tank.png"));
				gameObjects.push_back(new Tank(600.0f, 400.0f, 100.0f, 50, "tank.png"));

				gameObjects.push_back(new Wall(350.0f, 300.0f, "wall.png"));
				gameObjects.push_back(new Wall(400.0f, 300.0f, "wall.png"));
				gameObjects.push_back(new Wall(450.0f, 300.0f, "wall.png"));

				gameObjects.push_back(new DestructibleWall(500.0f, 300.0f, "destructible_wall.png"));

				gameObjects.push_back(new MedKit(300.0f, 100.0f, "medkit.png"));

				state = GAME;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				window.close();
			}
		}
		else if (state == GAME) {
			// Sterowanie graczem
			if (player && player->isActive()) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { player->move(0, deltaTime); currentDir = 0; }
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { player->move(1, deltaTime); currentDir = 1; }
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { player->move(2, deltaTime); currentDir = 2; }
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { player->move(3, deltaTime); currentDir = 3; }

				// Strza³ gracza -> flaga 'true' na koñcu konstruktora pocisku
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !spacePressed) {
					gameObjects.push_back(new Bullet(player->getX(), player->getY(), 400.0f, currentDir, "bullet.png", true));
					spacePressed = true;
				}
				else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
					spacePressed = false;
				}
			}

			// Aktualizacja pozycji obiektów i przechwytywanie pocisków od botów
			std::vector<Bullet*> newEnemyBullets;
			for (Entity* obj : gameObjects) {
				Bullet* enemyBullet = obj->update(deltaTime);
				if (enemyBullet) newEnemyBullets.push_back(enemyBullet);
				if (obj->getX() < 0 || obj->getX() > 800 || obj->getY() < 0 || obj->getY() > 600) obj->destroy();
			}
			for (Bullet* b : newEnemyBullets) gameObjects.push_back(b);

			// KOLIZJE: Pocisk -> Czo³g (Styl Battle City - pe³na wymiana ognia)
			for (Entity* objA : gameObjects) {
				Bullet* bullet = dynamic_cast<Bullet*>(objA);
				if (bullet && bullet->isActive()) {

					for (Entity* objB : gameObjects) {
						Tank* tank = dynamic_cast<Tank*>(objB);

						if (tank && tank->isActive() && tank->getBounds().intersects(bullet->getBounds())) {

							// 1. Przypadek: Pocisk zosta³ wystrzelony przez GRACZA
							if (bullet->isFromPlayer()) {
								if (tank == player) continue; // Gracz nie rani samego siebie

								// Trafienie w bota!
								tank->takeDamage(25);
								bullet->destroy();

								// Jeœli bot wybuch³, dodaj 100 punktów
								if (!tank->isActive()) {
									score += 100;
									std::cout << "Zniszczono wroga! Wynik: " << score << std::endl;
								}
								std::cout << "Trafienie! Czolg wroga oberwal!" << std::endl;
								break;
							}

							// 2. Przypadek: Pocisk zosta³ wystrzelony przez BOTA
							else {
								if (tank != player) continue; // Boty nie rani¹ innych botów

								// Trafienie w Ciebie! Tracisz HP!
								tank->takeDamage(25);
								bullet->destroy();

								std::cout << "Oberwales! Twoje HP spada!" << std::endl;
								break;
							}
						}
					}
				}
			}

			// KOLIZJE ZE ŒCIANAMI I POWER-UPAMI
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

				if (player && player->isActive() && tank && tank == player) {
					for (Entity* powerObj : gameObjects) {
						PowerUp* power = dynamic_cast<PowerUp*>(powerObj);
						if (power && power->isActive() && tank->getBounds().intersects(power->getBounds())) {
							power->applyEffect(tank);
							break;
						}
					}
				}
			}
			for (Entity* obj : gameObjects) {
				Tank* tank = dynamic_cast<Tank*>(obj);
				if (tank && tank->isActive()) {
					// Za³ó¿my bezpieczny rozmiar czo³gu (np. murek/tekstura ma ok. 32-50px)
					// Blokada lewej i prawej krawêdzi (ekran ma 800 px szerokoœci)
					if (tank->getX() < 20.0f) tank->move(1, deltaTime * 2.0f);      // Cofnij w prawo
					if (tank->getX() > 780.0f) tank->move(3, deltaTime * 2.0f);     // Cofnij w lewo

					// Blokada górnej i dolnej krawêdzi (ekran ma 600 px wysokoœci)
					if (tank->getY() < 50.0f) tank->move(2, deltaTime * 2.0f);      // Cofnij w dó³ (uwzglêdniamy miejsce na HUD)
					if (tank->getY() > 580.0f) tank->move(0, deltaTime * 2.0f);     // Cofnij w górê
				}
			}
			// Usuwanie martwych obiektów i zarz¹dzanie pamiêci¹
			for (auto it = gameObjects.begin(); it != gameObjects.end(); ) {
				if (!(*it)->isActive()) {
					if (*it == player) {
						player = nullptr;
						state = GAMEOVER; // Koniec gry po œmierci gracza
					}
					delete* it;
					it = gameObjects.erase(it);
				}
				else {
					++it;
				}
			}
		}
		else if (state == GAMEOVER) {
			// --- 2. OBS£UGA ZAPISU BINARNEGO I WYJ¥TKÓW PRZY KOÑCU GRY ---
			if (!scoreSaved) {
				if (score > highScore) {
					highScore = score;
					try {
						ScoreManager::saveHighScore(highScore, "highscore.dat");
						std::cout << "Zapisano nowy rekord binarnie!" << std::endl;
					}
					catch (const FileException& e) {
						std::cerr << "Blad krytyczny zapisu binarnego: " << e.what() << std::endl;
					}
				}
				scoreSaved = true;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				state = MENU;
			}
		}

		// --- RENDEROWANIE KLATKI GRAPHICS ---
		window.clear(sf::Color(50, 50, 50));
		if (state == MENU) {
			hud.drawMenu(window, highScore);
		}
		else {
			for (Entity* obj : gameObjects) obj->draw(window);
			hud.draw(window, player, score, highScore);
		}

		window.display();
	}

	for (Entity* obj : gameObjects) delete obj;
	return 0;
}
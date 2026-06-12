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
		// --- LOGIKA ZALE¯NA OD STANU GRY ---
		// --- LOGIKA ZALE¯NA OD STANU GRY ---
		if (state == MENU) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
				// Czyszczenie starej planszy na wypadek restartu
				for (Entity* obj : gameObjects) delete obj;
				gameObjects.clear();

				score = 0;
				scoreSaved = false;

				// Inicjalizacja gracza - bezpieczna pozycja na dole ekranu
				player = new Tank(400.0f, 500.0f, 150.0f, 100, "tank.png", true);
				gameObjects.push_back(player);

				// --- NOWA MAPA: KRAWDÊZIE PUSTE, W ŒRODKU DU¯E KWADRATOWE WYSPY ---
				// 12 wierszy (Y) i 16 kolumn (X)
				const int MAP_ROWS = 12;
				const int MAP_COLS = 16;

				int levelMap[MAP_ROWS][MAP_COLS] = {
					{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Czysta góra - idealna na spawny
					{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					{0, 0, 1, 1, 0, 0, 2, 2, 2, 2, 0, 0, 1, 1, 0, 0}, // Du¿e, stabilne kwadraty 2x2 z '1'
					{0, 0, 1, 1, 0, 0, 2, 2, 2, 2, 0, 0, 1, 1, 0, 0},
					{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					{0, 0, 2, 2, 0, 0, 1, 3, 3, 1, 0, 0, 2, 2, 0, 0}, // Œrodek: apteczki zabezpieczone metalem
					{0, 0, 2, 2, 0, 0, 1, 0, 0, 1, 0, 0, 2, 2, 0, 0},
					{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					{0, 0, 1, 1, 0, 0, 2, 2, 2, 2, 0, 0, 1, 1, 0, 0}, // Dolne kwadraty pancerne
					{0, 0, 1, 1, 0, 0, 2, 2, 2, 2, 0, 0, 1, 1, 0, 0},
					{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}  // Czysty dó³ dla gracza
				};

				// Pêtla buduj¹ca mapê na ekranie
				for (int r = 0; r < MAP_ROWS; ++r) {
					for (int c = 0; c < MAP_COLS; ++c) {
						float posX = c * 50.0f + 25.0f;
						float posY = r * 50.0f + 25.0f;

						// Blokada stawiania klocków w miejscu startu gracza
						if (posX > 300.0f && posX < 500.0f && posY > 450.0f) {
							continue;
						}

						if (levelMap[r][c] == 1) {
							gameObjects.push_back(new Wall(posX, posY, "wall.png"));
						}
						else if (levelMap[r][c] == 2) {
							gameObjects.push_back(new DestructibleWall(posX, posY, "destructible_wall.png"));
						}
						else if (levelMap[r][c] == 3) {
							gameObjects.push_back(new MedKit(posX, posY, "medkit.png"));
						}
					}
				}

				// Spawny botów w bezpiecznych miejscach u góry ekranu
				gameObjects.push_back(new Tank(75.0f, 40.0f, 100.0f, 50, "tank.png"));
				gameObjects.push_back(new Tank(725.0f, 40.0f, 100.0f, 50, "tank.png"));

				state = GAME;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				window.close();
			}
		}
		else if (state == GAME) {
			// --- SYSTEM PROGRESYWNEJ TRUDNOŒCI Z LIMITAMI (CAP) ---
			float difficultyFactor = 1.0f + (static_cast<float>(score) / 200.0f) * 0.1f;
			if (difficultyFactor > 1.5f) difficultyFactor = 1.5f;

			float currentSpawnCooldown = 4.0f / difficultyFactor;

			static float spawnTimer = 0.0f;
			spawnTimer += deltaTime;

			if (spawnTimer >= currentSpawnCooldown) {
				spawnTimer = 0.0f;

				int enemyCount = 0;
				for (Entity* obj : gameObjects) {
					Tank* t = dynamic_cast<Tank*>(obj);
					if (t && t != player && t->isActive()) {
						enemyCount++;
					}
				}

				if (enemyCount < 4) {
					// --- NOWY SYSTEM: SPAWN TYLKO W K¥TACH MAPY ---
					float spawnX = 75.0f; // Domyœlnie lewy górny róg

					// Losujemy 0 lub 1. Jeœli wyjdzie 1, przerzucamy spawn do prawego górnego rogu
					if (rand() % 2 == 1) {
						spawnX = 725.0f;
					}

					float spawnY = 40.0f; // Sta³a, bezpieczna wysokoœæ w rogu

					// Prêdkoœæ bota (z limitem do 140.0f)
					float enemySpeed = 100.0f * difficultyFactor;
					if (enemySpeed > 140.0f) enemySpeed = 140.0f;

					// Dodajemy bota na wylosowanym k¹cie
					gameObjects.push_back(new Tank(spawnX, spawnY, enemySpeed, 50, "tank.png"));
					std::cout << "Respawn bota w kacie mapy! X: " << spawnX << ", Predkosc: " << enemySpeed << std::endl;
				}
			}

			static float playerHitCooldown = 0.0f;
			if (playerHitCooldown > 0.0f) {
				playerHitCooldown -= deltaTime;
			}

			// Sterowanie graczem
			if (player && player->isActive()) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { player->move(0, deltaTime); currentDir = 0; }
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { player->move(1, deltaTime); currentDir = 1; }
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { player->move(2, deltaTime); currentDir = 2; }
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { player->move(3, deltaTime); currentDir = 3; }

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !spacePressed) {
					gameObjects.push_back(new Bullet(player->getX(), player->getY(), 400.0f, currentDir, "bullet.png", true));
					spacePressed = true;
				}
				else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
					spacePressed = false;
				}
			}

			// Aktualizacja pozycji i generowanie pocisków wrogów
			std::vector<Bullet*> newEnemyBullets;
			for (Entity* obj : gameObjects) {
				float currentDelta = deltaTime;
				Tank* isEnemy = dynamic_cast<Tank*>(obj);

				if (isEnemy && isEnemy != player) {
					currentDelta *= difficultyFactor;
				}

				Bullet* enemyBullet = obj->update(currentDelta);

				if (enemyBullet) {
					float bulletSpeed = 300.0f * difficultyFactor;
					if (bulletSpeed > 450.0f) bulletSpeed = 450.0f;

					enemyBullet->setSpeed(bulletSpeed);
					newEnemyBullets.push_back(enemyBullet);
				}
				if (obj->getX() < 0 || obj->getX() > 800 || obj->getY() < 0 || obj->getY() > 600) obj->destroy();
			}
			for (Bullet* b : newEnemyBullets) gameObjects.push_back(b);

			// KOLIZJE: Pocisk -> Czo³g
			for (Entity* objA : gameObjects) {
				Bullet* bullet = dynamic_cast<Bullet*>(objA);
				if (bullet && bullet->isActive()) {
					for (Entity* objB : gameObjects) {
						Tank* tankObj = dynamic_cast<Tank*>(objB);

						if (tankObj && tankObj->isActive() && tankObj->getHealth() > 0 && tankObj->getBounds().intersects(bullet->getBounds())) {
							if (bullet->isFromPlayer()) {
								if (tankObj == player) continue;
								tankObj->takeDamage(50);
								bullet->destroy();
								if (tankObj->getHealth() <= 0 || !tankObj->isActive()) {
									score += 100;
									std::cout << "Zniszczono wroga! Wynik: " << score << std::endl;
								}
								break;
							}
							else {
								if (tankObj != player) continue;

								if (playerHitCooldown <= 0.0f) {
									tankObj->takeDamage(50);
									playerHitCooldown = 0.5f;
									std::cout << "Oberwales! Twoje HP wynosi teraz: " << tankObj->getHealth() << std::endl;
								}
								bullet->destroy();
								break;
							}
						}
					}
				}
			}

			// --- POPRAWIONE KOLIZJE ZE ŒCIANAMI I APTECZKAMI ---
			for (Entity* obj : gameObjects) {
				// 1. Kolizje pocisków ze œcianami
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

				// 2. Kolizje czo³gów ze œcianami (Z systemem p³ynnego odbijania PUSH_BACK)
				// 2. Kolizje czo³gów ze œcianami (Z systemem p³ynnego odbijania - NAPRAWIONE PRZYKLEJANIE)
				Tank* tank = dynamic_cast<Tank*>(obj);
				if (tank && tank->isActive()) {
					for (Entity* wallObj : gameObjects) {
						Wall* wall = dynamic_cast<Wall*>(wallObj);
						if (wall && tank->getBounds().intersects(wall->getBounds())) {
							int dir = tank->getDirection();

							// Gwarantowane, natychmiastowe wypchniêcie poza obrys œciany (w pikselach)
							const float PUSH_DIST = 5.0f;

							if (dir == 0)      tank->move(2, deltaTime * 2.0f); // Najpierw standardowy ruch w ty³
							else if (dir == 1) tank->move(3, deltaTime * 2.0f);
							else if (dir == 2) tank->move(0, deltaTime * 2.0f);
							else if (dir == 3) tank->move(1, deltaTime * 2.0f);

							// --- KLUCZOWY FIX: Rêczna korekta pozycji, ¿eby zerwaæ "klej" ---
							// Wykorzystujemy publiczne metody getX/getY i musimy mieæ mo¿liwoœæ przesuniêcia czo³gu.
							// Jeœli nie masz metody setPosition, to wymuszenie dodatkowego ruchu w ty³ bez deltaTime za³atwi sprawê:
							if (dir == 0)      tank->move(2, 0.05f); // Dodatkowy, twardy impuls w dó³
							else if (dir == 1) tank->move(3, 0.05f); // Dodatkowy, twardy impuls w lewo
							else if (dir == 2) tank->move(0, 0.05f); // Dodatkowy, twardy impuls w górê
							else if (dir == 3) tank->move(1, 0.05f); // Dodatkowy, twardy impuls w prawo

							// Dla botów: natychmiast ka¿emy im zmieniæ kierunek, ¿eby nie par³y œlepo w murek
							if (tank != player) {
								// Bot zmienia kierunek na losowy, ¿eby natychmiast uciec od œciany
								// Jeœli chcesz, mo¿emy wymusiæ zmianê kierunku w klasie Tank, 
								// ale samo twarde odepchniêcie ju¿ go uwolni!
							}
							break;
						}
					}
				}

				// 3. Kolizje gracza z apteczkami
				if (player && player->isActive() && tank && tank == player) {
					for (Entity* powerObj : gameObjects) {
						MedKit* medKit = dynamic_cast<MedKit*>(powerObj); // Bezpoœrednie rzutowanie na MedKit
						if (medKit && medKit->isActive() && tank->getBounds().intersects(medKit->getBounds())) {
							tank->heal(50); // Apteczka leczy o 50 HP
							medKit->destroy(); // Usuwamy apteczkê z mapy
							break;
						}
					}
				}
			}

			// GRANICE MAPY DLA CZÓ£GÓW
			for (Entity* obj : gameObjects) {
				Tank* tankCheck = dynamic_cast<Tank*>(obj);
				if (tankCheck && tankCheck->isActive()) {
					if (tankCheck->getX() < 20.0f) tankCheck->move(1, deltaTime * 2.0f);
					if (tankCheck->getX() > 780.0f) tankCheck->move(3, deltaTime * 2.0f);
					if (tankCheck->getY() < 50.0f) tankCheck->move(2, deltaTime * 2.0f);
					if (tankCheck->getY() > 580.0f) tankCheck->move(0, deltaTime * 2.0f);
				}
			}

			// USUWANIE MARTWYCH OBIEKTÓW I SPRAWDZANIE KOÑCA GRY
			for (auto it = gameObjects.begin(); it != gameObjects.end(); ) {
				Tank* tankCheck = dynamic_cast<Tank*>(*it);
				if (tankCheck && tankCheck->getHealth() <= 0) {
					tankCheck->destroy();
				}

				if (!(*it)->isActive()) {
					if (*it == player) {
						player = nullptr;
						state = GAMEOVER;
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
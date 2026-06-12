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

	// --- 1. OBS£UGA WYJ•TK”W I PLIK”W BINARNYCH NA STARCE ---
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

		// --- LOGIKA ZALEØNA OD STANU GRY ---
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

				// --- POPRAWIONA MAPA: 4 PANCERNE KWADRATY + LUèNE åCIANKI W åRODKU ---
				// 1 - twarda, 2 - do rozwalenia, 0 - puste pole
				const int MAP_ROWS = 12;
				const int MAP_COLS = 16;

				int levelMap[MAP_ROWS][MAP_COLS] = {
					{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // GÛra czysta (pod spawny botÛw)
					{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					{0, 0, 1, 1, 0, 0, 2, 2, 2, 2, 0, 0, 1, 1, 0, 0}, // 4 Kwadraty pancerne (1) w rogach
					{0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0}, // Luüniejsze úcianki zniszczalne (2) w úrodku
					{0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0, 2, 1, 0, 0, 1, 2, 0, 0, 0, 0, 0}, // Centralna sekcja
					{0, 0, 0, 0, 0, 2, 1, 0, 0, 1, 2, 0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0},
					{0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
					{0, 0, 1, 1, 0, 0, 2, 2, 2, 2, 0, 0, 1, 1, 0, 0},
					{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}  // DÛ≥ czysty dla gracza
				};

				// PÍtla budujπca mapÍ na ekranie
				for (int r = 0; r < MAP_ROWS; ++r) {
					for (int c = 0; c < MAP_COLS; ++c) {
						float posX = c * 50.0f + 25.0f;
						float posY = r * 50.0f + 25.0f;

						if (posX > 300.0f && posX < 500.0f && posY > 450.0f) {
							continue;
						}

						if (levelMap[r][c] == 1) {
							gameObjects.push_back(new Wall(posX, posY, "wall.png"));
						}
						else if (levelMap[r][c] == 2) {
							gameObjects.push_back(new DestructibleWall(posX, posY, "destructible_wall.png"));
						}
					}
				}

				// Startowe boty w bezpiecznych kπtach
				gameObjects.push_back(new Tank(75.0f, 40.0f, 100.0f, 50, "enemy_tank.png"));
				gameObjects.push_back(new Tank(725.0f, 40.0f, 100.0f, 50, "enemy_tank.png"));

				state = GAME;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				window.close();
			}
		}
		else if (state == GAME) {
			// --- SYSTEM PROGRESYWNEJ TRUDNOåCI Z LIMITAMI (CAP) ---
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
					// --- SPAWN TYLKO W K•TACH MAPY ---
					float spawnX = 75.0f;
					if (rand() % 2 == 1) {
						spawnX = 725.0f;
					}
					float spawnY = 40.0f;

					float enemySpeed = 100.0f * difficultyFactor;
					if (enemySpeed > 140.0f) enemySpeed = 140.0f;

					gameObjects.push_back(new Tank(spawnX, spawnY, enemySpeed, 50, "enemy_tank.png"));
					std::cout << "Respawn bota! Predkosc: " << enemySpeed << " (Mnoznik: " << difficultyFactor << ")" << std::endl;
				}
			}

			// --- SYSTEM LOSOWEGO SPAWNU APTECZEK CO 10 SEKUND ---
			static float medKitTimer = 0.0f;
			medKitTimer += deltaTime;

			if (medKitTimer >= 10.0f) {
				medKitTimer = 0.0f;

				int medKitCount = 0;
				for (Entity* obj : gameObjects) {
					if (dynamic_cast<MedKit*>(obj) && obj->isActive()) {
						medKitCount++;
					}
				}

				if (medKitCount < 2) {
					float medKitX = 0.0f;
					float medKitY = 0.0f;
					bool validPosition = false;

					while (!validPosition) {
						medKitX = static_cast<float>(100 + rand() % 600);
						medKitY = static_cast<float>(100 + rand() % 400);

						validPosition = true;

						// Sprawdzamy, czy apteczka nie nak≥ada siÍ na jakπkolwiek úcianÍ (zwyk≥π lub zniszczalnπ)
						for (Entity* wallObj : gameObjects) {
							Wall* wall = dynamic_cast<Wall*>(wallObj);
							if (wall) {
								sf::FloatRect testBounds(medKitX - 15.0f, medKitY - 15.0f, 30.0f, 30.0f);
								if (testBounds.intersects(wall->getBounds())) {
									validPosition = false;
									break;
								}
							}
						}
					}

					gameObjects.push_back(new MedKit(medKitX, medKitY, "medkit.png"));
					std::cout << "Pojawila sie nowa apteczka na pozycji X: " << medKitX << ", Y: " << medKitY << std::endl;
				}
			}

			// Ochrona gracza przed úmierciπ na hita
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

			// Aktualizacja pozycji i strzelanie wrogÛw
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

			// KOLIZJE: Pocisk -> Czo≥g
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

			// KOLIZJE ZE åCIANAMI I APTECZKAMI
			for (Entity* obj : gameObjects) {
				// Pociski -> åciany
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

				// Czo≥gi -> åciany (Z systemem p≥ynnego odpychania)
				Tank* tank = dynamic_cast<Tank*>(obj);
				if (tank && tank->isActive()) {
					for (Entity* wallObj : gameObjects) {
						Wall* wall = dynamic_cast<Wall*>(wallObj);
						if (wall && tank->getBounds().intersects(wall->getBounds())) {
							int dir = tank->getDirection();

							if (dir == 0)      tank->move(2, deltaTime * 2.0f);
							else if (dir == 1) tank->move(3, deltaTime * 2.0f);
							else if (dir == 2) tank->move(0, deltaTime * 2.0f);
							else if (dir == 3) tank->move(1, deltaTime * 2.0f);

							if (dir == 0)      tank->move(2, 0.05f);
							else if (dir == 1) tank->move(3, 0.05f);
							else if (dir == 2) tank->move(0, 0.05f);
							else if (dir == 3) tank->move(1, 0.05f);
							break;
						}
					}
				}

				// Gracz -> Apteczki
				if (player && player->isActive() && tank && tank == player) {
					for (Entity* powerObj : gameObjects) {
						MedKit* medKit = dynamic_cast<MedKit*>(powerObj);
						if (medKit && medKit->isActive() && tank->getBounds().intersects(medKit->getBounds())) {
							tank->heal(50);
							medKit->destroy();
							break;
						}
					}
				}
			}

			// GRANICE MAPY DLA CZ”£G”W
			for (Entity* obj : gameObjects) {
				Tank* tankCheck = dynamic_cast<Tank*>(obj);
				if (tankCheck && tankCheck->isActive()) {
					if (tankCheck->getX() < 25.0f)  tankCheck->move(1, deltaTime * 2.0f);
					if (tankCheck->getX() > 775.0f) tankCheck->move(3, deltaTime * 2.0f);
					if (tankCheck->getY() < 25.0f)  tankCheck->move(2, deltaTime * 2.0f);
					if (tankCheck->getY() > 575.0f) tankCheck->move(0, deltaTime * 2.0f);
				}
			}

			// USUWANIE MARTWYCH OBIEKT”W
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
			// --- 2. OBS£UGA ZAPISU BINARNEGO I WYJ•TK”W PRZY KO—CU GRY ---
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

			// --- RESTART GRY PO ENTERZE NA IDENTYCZNEJ MAPIE ---
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
				for (Entity* obj : gameObjects) delete obj;
				gameObjects.clear();

				score = 0;
				scoreSaved = false;

				player = new Tank(400.0f, 500.0f, 150.0f, 100, "tank.png", true);
				gameObjects.push_back(player);

				const int MAP_ROWS = 12;
				const int MAP_COLS = 16;

				int levelMap[MAP_ROWS][MAP_COLS] = {
					{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					{0, 0, 1, 1, 0, 0, 2, 2, 2, 2, 0, 0, 1, 1, 0, 0},
					{0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
					{0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0, 2, 1, 0, 0, 1, 2, 0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0, 2, 1, 0, 0, 1, 2, 0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0},
					{0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
					{0, 0, 1, 1, 0, 0, 2, 2, 2, 2, 0, 0, 1, 1, 0, 0},
					{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
				};

				for (int r = 0; r < MAP_ROWS; ++r) {
					for (int c = 0; c < MAP_COLS; ++c) {
						float posX = c * 50.0f + 25.0f;
						float posY = r * 50.0f + 25.0f;

						if (posX > 300.0f && posX < 500.0f && posY > 450.0f) continue;

						if (levelMap[r][c] == 1) {
							gameObjects.push_back(new Wall(posX, posY, "wall.png"));
						}
						else if (levelMap[r][c] == 2) {
							gameObjects.push_back(new DestructibleWall(posX, posY, "destructible_wall.png"));
						}
					}
				}

				gameObjects.push_back(new Tank(75.0f, 40.0f, 100.0f, 50, "tank.png"));
				gameObjects.push_back(new Tank(725.0f, 40.0f, 100.0f, 50, "tank.png"));

				state = GAME;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				state = MENU;
			}
		}

		// --- RENDEROWANIE KLATKI ---
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
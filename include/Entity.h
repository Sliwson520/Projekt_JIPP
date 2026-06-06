#pragma once
#include <SFML/Graphics.hpp>
#include "API.h"

class ENGINE_API Entity {
protected:
	float x, y;
	float speed;
	bool active;
public:
	Entity(float startX, float startY, float startSpeed);
	virtual ~Entity()=default;
	virtual void update(float deltaTime) = 0;
	virtual void draw(sf::RenderWindow& window) = 0;
	float getX() const { return x; }
	float getY() const { return y; }
	bool isActive() const { return active; }
	void destroy() { active = false; }
};

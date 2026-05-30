#pragma once
#ifdef GAMEENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

class ENGINE_API Entity {
protected:
	float x, y;
	float speed;
	int direction;
public:
	Entity(float startX, float startY, float startSpeed);
	virtual ~Entity()=default;
	virtual void update(float deltaTime) = 0;
	float getX() const { return x; }
	float getY() const { return y; }
	int getDirection() const { return direction; }
};

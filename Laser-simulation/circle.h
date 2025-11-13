#pragma once
#include <SFML/Graphics.hpp>

class Circle
{
public:
	Circle(sf::CircleShape circle, char inType);
	void draw(sf::RenderWindow& window);
	bool move(bool leftEvent, bool rightEvent, sf::Vector2f pos, sf::Vector2f lastPos);
	bool reflects();
	sf::FloatRect hitbox();
	double x, y, r;

private:
	sf::CircleShape body;
	bool isReflective;
	bool isMoving = false;
	bool isRotating = false;
};


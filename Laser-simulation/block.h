#pragma once
#include <SFML/Graphics.hpp>

class Block
{
public:
	Block(sf::RectangleShape rect, char inType);
	bool move(bool leftEvent, bool rightEvent, sf::Vector2f pos, sf::Vector2f lastPos);
	void draw(sf::RenderWindow& window);
	sf::FloatRect hitbox();
	bool reflects();	
	double m, q;
	sf::Vector2f p1, p2;

private:
	sf::RectangleShape body;
	bool isReflective;
	bool isMoving = false;
	bool isRotating = false;
};
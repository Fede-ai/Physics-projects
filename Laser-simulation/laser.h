#pragma once
#include <SFML/Graphics.hpp>
#include <stack>
#include "block.h"
#include "circle.h"

class Laser
{
public:
	Laser(sf::Vector2f pos);
	bool move(bool leftEvent, bool rightEvent, sf::Vector2f pos, sf::Vector2f lastPos);
	void updateLaser(std::vector<Block> blocks, std::vector<Circle> circles);
	void drawBody(sf::RenderWindow& window);
	void drawLaser(sf::RenderWindow& window);
	sf::FloatRect hitbox();

private:
	std::pair<sf::Vector2f, int> findCollision(std::vector<Block> blocks, std::vector<Circle> circles, sf::Vector2f start, double ang, int exclude);
	double findNewAngle(double angBefore, Block wall);
	double findNewAngle(double angBefore, Circle circle, sf::Vector2f i);

	bool isMoving = false;
	bool isRotating = false;

	sf::RectangleShape body;
	sf::VertexArray laser;
};
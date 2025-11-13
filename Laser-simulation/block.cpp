#include "block.h"
constexpr auto PI = 3.14159265358979323846;

Block::Block(sf::RectangleShape rect, char inType)
{
	body = rect;

	if (inType == 'r')
	{
		body.setFillColor(sf::Color::Green);
		isReflective = true;
	}
	else if (inType == 'n')
	{
		body.setFillColor(sf::Color::Red);
		isReflective = false;
	}

	p1 = rect.getPosition();
	p2 = rect.getPosition();
	p2.x += cos(rect.getRotation() * PI / 180) * rect.getSize().x;
	p2.y += sin(rect.getRotation() * PI / 180) * rect.getSize().x;

	m = (p2.y - p1.y) / (p2.x - p1.x);
	q = p1.y - m * p1.x;
}

bool Block::move(bool leftEvent, bool rightEvent, sf::Vector2f pos, sf::Vector2f lastPos)
{
	bool needUpdate = false;

	if (leftEvent)
		isMoving = true;
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && isMoving)
		isMoving = false;

	if (isMoving)
	{
		needUpdate = true;
		body.move(pos - lastPos);
	}

	if (rightEvent)
		isRotating = true;
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Right) && isRotating)
		isRotating = false;

	if (isRotating)
	{
		needUpdate = true;
		sf::Vector2f diff = pos - body.getPosition();
		double ang = atan(diff.y / diff.x) * 180 / PI;
		if (pos.x < body.getPosition().x)
			ang -= 180;

		if (ang < 0)
			ang += 360;

		body.setRotation(ang);
		body.setSize(sf::Vector2f(sqrt(std::pow(diff.x, 2) + std::pow(diff.y, 2)), body.getSize().y));
	}

	if (needUpdate)
	{
		p1 = body.getPosition();
		p2 = body.getPosition();
		p2.x += cos(body.getRotation() * PI / 180) * body.getSize().x;
		p2.y += sin(body.getRotation() * PI / 180) * body.getSize().x;

		m = (p2.y - p1.y) / (p2.x - p1.x);
		q = p1.y - m * p1.x;
	}

	return needUpdate;
}

void Block::draw(sf::RenderWindow& window)
{
	window.draw(body);
}

sf::FloatRect Block::hitbox()
{
	return body.getGlobalBounds();
}

bool Block::reflects()
{
	return isReflective;
}

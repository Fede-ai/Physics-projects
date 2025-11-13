#include "laser.h"
#include <iostream>
constexpr auto PI = 3.14159265358979323846;

Laser::Laser(sf::Vector2f pos)
{
	body.setSize(sf::Vector2f(60, 20));
	body.setOrigin(30, 10);	
	body.setPosition(pos);
	body.setFillColor(sf::Color(30, 30, 30));

	laser.setPrimitiveType(sf::LineStrip);
}

bool Laser::move(bool leftEvent, bool rightEvent, sf::Vector2f pos, sf::Vector2f lastPos)
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
	}

	return needUpdate;
}

void Laser::updateLaser(std::vector<Block> blocks, std::vector<Circle> circles)
{
	auto formatAng = [](double ang) {
		while (ang < 0)
			ang += 360;

		while (ang >= 360)
			ang -= 360;

		return ang;
	};

	int nBounces = 0;
	laser.clear();
	laser.append(body.getPosition());
	double ang = body.getRotation();
	int lastBounceObj = -1;

	while (true)
	{
		if (ang == 0 || ang == 90 || ang == 180 || ang == 270)
			ang += 0.05;

		const auto info = findCollision(blocks, circles, laser[laser.getVertexCount()-1].position, ang, lastBounceObj);
		laser.append(info.first);

		lastBounceObj = info.second;
		if (lastBounceObj == -1)
			break;

		nBounces++;
		if (lastBounceObj < blocks.size())
			ang = formatAng(findNewAngle(ang, blocks[lastBounceObj]));
		else 
			ang = formatAng(findNewAngle(ang, circles[lastBounceObj - blocks.size()], info.first));

		if (nBounces >= 100'000)
		{
			nBounces = -1;
			break;
		}
	}

	if (nBounces == -1)
		std::cout << "bounces > 100'000\n";
	else
		std::cout << "bounces = " << nBounces << "\n";
}

void Laser::drawBody(sf::RenderWindow& window)
{
	window.draw(body);
}
void Laser::drawLaser(sf::RenderWindow& window)
{
	window.draw(laser);
}
sf::FloatRect Laser::hitbox()
{
	return body.getGlobalBounds();
}

std::pair<sf::Vector2f, int> Laser::findCollision(std::vector<Block> blocks, std::vector<Circle> circles, sf::Vector2f start, double ang, int exclude)
{
	auto intersection = [](double m1, double q1, double m2, double q2) {
		double x = (q2 - q1) / (m1 - m2);
		double y = m2 * x + q2;

		return sf::Vector2f(x, y);
	};
	auto contains = [](sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p) {
		double minX = std::min(p1.x, p2.x);
		double maxX = std::max(p1.x, p2.x);
		double minY = std::min(p1.y, p2.y);
		double maxY = std::max(p1.y, p2.y);

		//check the point is within the bounding box of the segment
		if ((p.x >= minX && p.x <= maxX) || (p.y >= minY && p.y <= maxY))
			return true;

		return false;
	};
	auto dist = [](sf::Vector2f p1, sf::Vector2f p2) {
		double dx = p2.x - p1.x;
		double dy = p2.y - p1.y;
		return sqrt(dx * dx + dy * dy);
	};

	//-1 means that the laser will not reflect,
	//otherwise return the index of the wall with wich the laser collides
	int nObj = -1;
	sf::Vector2f closest;
	bool collided = false;
	
	double mLaser = tan(ang * PI / 180);
	double qLaser = - mLaser * start.x + start.y;
	
	for (int i = 0; i < blocks.size(); i++)
	{
		if (i == exclude)
			continue;

		sf::Vector2f inter = intersection(mLaser, qLaser, blocks[i].m, blocks[i].q);
		if (((ang < 90 || ang > 270) && inter.x > start.x) ||
			(ang > 90 && ang < 270 && inter.x < start.x) ||
			(ang == 270 && inter.y < start.y) || (ang == 90 && inter.y > start.y))
		{
			if (contains(blocks[i].p1, blocks[i].p2, inter))
			{
				if (collided == false)
				{
					closest = inter;
					collided = true;
					if (blocks[i].reflects())
						nObj = i;
					else
						nObj = -1;
				}
				else if (dist(start, inter) < dist(start, closest))
				{
					closest = inter;
					if (blocks[i].reflects())
						nObj = i;
					else
						nObj = -1;
				}
			}
		}
	}

	for (int i = 0; i < circles.size(); i++)
	{
		if (i + blocks.size() == exclude)
			continue;

		sf::Vector2f inter, other;
		double A = 1 + mLaser * mLaser;
		double B = 2 * (mLaser * qLaser - mLaser * circles[i].y - circles[i].x);
		double C = circles[i].x * circles[i].x + circles[i].y * circles[i].y - circles[i].r * circles[i].r + qLaser * qLaser - 2 * qLaser * circles[i].y;
		double delta = B * B - 4 * A * C;

		if (delta == 0) {
			inter.x = -B / (2 * A);
			inter.y = mLaser * inter.x + qLaser;
		}
		else if (delta > 0) {
			inter.x = (-B + sqrt(delta)) / (2 * A);
			inter.y = mLaser * inter.x + qLaser;

			other.x = (-B - sqrt(delta)) / (2 * A);
			other.y = mLaser * other.x + qLaser;

			if (dist(start, other) < dist(start, inter))
				inter = other;
		}
		else
		{
			continue;
		}

		if (((ang < 90 || ang > 270) && inter.x > start.x) ||
			(ang > 90 && ang < 270 && inter.x < start.x) ||
			(ang == 270 && inter.y < start.y) || (ang == 90 && inter.y > start.y))
		{
			if (collided == false)
			{
				closest = inter;
				collided = true;
				if (circles[i].reflects())
					nObj = blocks.size() + i;
				else
					nObj = -1;
			}
			else if (dist(start, inter) < dist(start, closest))
			{
				closest = inter;
				if (circles[i].reflects())
					nObj = blocks.size() + i;
				else
					nObj = -1;
			}
		}
	}

	if (!collided)
	{
		if (ang == 0 || ang == 360)
			closest = sf::Vector2f(1920, start.y);
		else if (ang == 180)
			closest = sf::Vector2f(0, start.y);
		else if (ang < 180)
			closest = sf::Vector2f((1080 - qLaser) / mLaser, 1080);
		else if (ang > 180)
			closest = sf::Vector2f(-qLaser / mLaser, 0);
	}

	return std::make_pair(closest, nObj);
}

double Laser::findNewAngle(double angBefore, Block block)
{
	double blockAng = atan(block.m) * 180 / PI;
	if (blockAng < 0)
		blockAng = 180 + blockAng;

	return blockAng * 2 - angBefore;
}
double Laser::findNewAngle(double angBefore, Circle circle, sf::Vector2f i)
{
	double mLine = -1 / ((i.y - circle.y) / (i.x - circle.x));
	double lineAng = atan(mLine) * 180 / PI;
	if (lineAng < 0)
		lineAng = 180 + lineAng;
	
	return lineAng * 2 - angBefore;
}

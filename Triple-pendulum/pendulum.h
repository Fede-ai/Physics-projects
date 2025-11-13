#pragma once
#include <SFML/Graphics.hpp>
#include <stack>

class Pendulum
{
public:
	Pendulum();
	int run();

private:
	void move();
	void draw();

	void moveSecond();
	void moveThird();

	sf::RenderWindow window;
	sf::Vector2i windowSize;
	sf::ContextSettings settings;

	sf::VertexArray background;
	sf::Clock clock;
	int lastFps = 0;
	int fps = 0;
	
	sf::Font font;
	sf::Text text;

	sf::CircleShape firstStart;
	sf::RectangleShape firstBody;
	sf::CircleShape firstEnd;
	sf::Vector2i firstSize = sf::Vector2i(250, 50);
	double firstAng = 0;
	double firstSpeed = 1; //global speed, 1 = 1 rotation/6 seconds

	sf::CircleShape secondStart;
	sf::RectangleShape secondBody;
	sf::CircleShape secondEnd;
	sf::Vector2i secondSize = sf::Vector2i(180, 40);
	double secondAng = 0;
	double actualSecondAng = 0;
	double secondSpeed = -2.5; //it does x rotations around the first arm for each first arm rotation
	std::vector<sf::Vertex> secondTray;
	bool drawSecond = true;

	sf::CircleShape thirdStart;
	sf::RectangleShape thirdBody;
	sf::CircleShape thirdEnd;
	sf::Vector2i thirdSize = sf::Vector2i(100, 30);
	double thirdAng = 0;
	double actualThirdAng = 0;
	double thirdSpeed = 5; //it does x rotations around the second arm for each first arm rotation
	std::vector<sf::Vertex> thirdTray;
	bool drawThird = true;
};


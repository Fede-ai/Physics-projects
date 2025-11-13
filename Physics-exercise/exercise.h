#pragma once
#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>

class Exercise
{
public:
	Exercise();
	int run();

private:
	void updateAndDrawObj();
	void moveGround();

	static size_t time();

	sf::RenderWindow window;
	bool isFullscreen = false;
	bool canFullscreen = false;
	size_t lastTime = time();
	int exerciseTime = 0;
	bool isPaused = true;
	const double meter = 300;
	double speed = 1;

	sf::Vector2f lastMousePos;

	sf::VertexArray background;
	sf::VertexArray ground;
	sf::RectangleShape water;
	sf::VertexArray hydro;

	sf::Texture springTexture;
	sf::RectangleShape spring;
	sf::CircleShape sphere1;
	sf::RectangleShape line1;
	sf::CircleShape sphere2;
	sf::RectangleShape line2;
	sf::CircleShape sphere3;
	sf::RectangleShape line3;
	sf::CircleShape sphere4;
	sf::CircleShape sphere5;
	sf::RectangleShape pedana;
	sf::RectangleShape pistone;
	sf::RectangleShape cube;
	sf::CircleShape sphere6;

	sf::Font font;
	sf::Text staticTxt;
};
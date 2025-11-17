#pragma once
#include <SFML/Graphics.hpp>

class Pendulum {
public:
	Pendulum();
	void update();
	sf::Image renderGraph(int width, int height);

	void moveGraph();
	void applyZoom(float delta);

	void setPendulum(int x, int y, int width, int height);
	void forceRerender();

	static constexpr double pi = 3.14159265359;

	sf::Vector2<double> pendulum = { 0.25, 2 };
	int drag = 250;

	bool isUpdating = false;
	bool isMovingGraph = false;

private:
	sf::Image renderAxes(int width, int height) const;

	sf::Vector2f anchor = sf::Vector2f(0, 0);
	sf::Vector2f defaultSize = sf::Vector2f(0, 0);

	sf::Vector2i unit = sf::Vector2i(0, 0);
	float zoom = 1;

	std::vector<sf::Vector2<double>> trail;
	sf::Vector2i lastMousePos = sf::Mouse::getPosition();

	double length = 1;
	bool hasMoved = true;
	sf::Image lastImg;
};


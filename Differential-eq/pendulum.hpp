#pragma once
#include <SFML/Graphics.hpp>

class Pendulum {
public:
	Pendulum();
	void update(int fps);
	sf::Image renderGraph(int width, int height);

	void moveGraph();
	void applyZoom(float delta);

	void setPendulum(int x, int y, int width, int height);
	void forceRerender();

	static constexpr double pi = 3.14159265359;

	//pendulum.x = angle [rad], pendulum.y = angular velocity [rad/s]
	sf::Vector2<double> pendulum = { -1.5 * pi, 1.5 * pi + .1};
	int drag = 400;

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

	const double length = 1;
	bool hasMoved = true;
	sf::Image lastImg;
};


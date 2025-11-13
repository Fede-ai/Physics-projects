#include <SFML/Graphics.hpp>
#include "block.h"
#include "laser.h"
#include "circle.h"

class Simulation
{
public:
	Simulation();
	int run();

private:
	void move();
	void draw();

	sf::RenderWindow window;
	sf::Vector2i windowSize;
	sf::Vector2f viewSize = sf::Vector2f(1920, 1080);
	sf::Vector2f lastMousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	
	sf::VertexArray background;
	sf::RectangleShape wall;
	sf::CircleShape circle;

	char buildingType = ' ';
	bool isBuildingWall = false;
	bool isBuildingCircle = false;
	bool canSelectBuilding = false;
	double wallThickness = 4;

	bool leftEvent = false;
	bool rightEvent = false;
	bool updateNeeded = true;

	std::vector<Block> blocks;
	std::vector<Circle> circles;
	std::vector<Laser> lasers;
};
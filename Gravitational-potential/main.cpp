#include "engine.hpp"
#include "gravity.hpp"

int main() {
	GravitySimulator sim;
	auto potentialDist = [&sim](double x, double z) {
		return sim.getPotentialAtPoint(x, z);
		};

	//first two bodies have mass, the third is massless
	std::vector<Body> bodies = { 
		Body({0, -20, 0}, {}, 500),
		Body({0, -20, 80}, {18, 0, 0}, 10),
		Body({0, -20, 70}, { 12, 0, 0 })
	};
	sim.addBodies(bodies);

	bool isRunning = false;
	const unsigned int fps = 60;
	Engine3D engine(fps);
	const float width = sf::VideoMode::getFullscreenModes()[0].size.x * 2 / 3.f;
	auto& window = engine.createWindow(sf::Vector2u(width, width * 9.f / 16.f));

	while (window.isOpen()) {
		int status = engine.handleEvents();
		if (status == 1)
			isRunning = !isRunning;
		
		if (isRunning)
			sim.step(1.0 / fps);

		window.clear(sf::Color(10, 10, 10));
		engine.renderPotentialField(potentialDist);

		engine.renderBodies(bodies);

		window.display();
	}

	return 0;
}
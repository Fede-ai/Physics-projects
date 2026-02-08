#include "engine.hpp"
#include "gravity.hpp"
#include <chrono>
#include <iostream>

int main() {
	GravitySimulator sim(500, 99 * 4);

	std::vector<Vec3> stabilityPoints;
	std::vector<Body> bodies = { 
		Body({ 0, 0, 0 }, { 21.0 / 25, 0, 0 }, 500),
		Body({ 0, 0, 80 }, { 18, 0, 0 }, 10),
		Body({ 0, 0, 70 }, { 25, 0, 0 }),
		Body({ 0, 0, -85 }, { -20, 0, 0 }, 30)
	};
	sim.addBodies(bodies);
	sim.calculateStabilityPoints(stabilityPoints);

	bool isRunning = false;
	const unsigned int fps = 60;
	Engine3D engine(fps, 500, 50);
	const float width = sf::VideoMode::getFullscreenModes()[0].size.x * 2 / 3.f;
	auto& window = engine.createWindow(sf::Vector2u(width, width * 9.f / 16.f));

	size_t frame = 0;
	while (window.isOpen()) {
		int status = engine.handleEvents();
		if (status == 1)
			isRunning = !isRunning;

		if (isRunning)
			sim.step(1.0 / fps);

		window.clear(sf::Color(10, 10, 10));
		
		//taking a lot of time
		engine.renderPotentialField(sim);

		engine.renderBodies(bodies);

		//taking even more time
		if (isRunning)
			sim.calculateStabilityPoints(stabilityPoints);

		engine.renderPoints(stabilityPoints);

		window.display();

		if (frame++ % (fps / 2) == 0) {
			Vec3 momentum(0, 0, 0);
			for (const auto& b : bodies)
				momentum += b.mass * b.velocity;
			std::cout << "Momentum: <" << momentum.x << ", " << momentum.y << ", " << momentum.z << ">\n";
		}
	}

	return 0;
}
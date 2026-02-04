#include "engine.hpp"
#include "gravity.hpp"

int main() {
	GravitySimulator sim;
	auto potentialDist = [&sim](double x, double z) {
		return sim.getPotentialAtPoint(x, z);
		};

	const float width = sf::VideoMode::getFullscreenModes()[0].size.x * 2 / 3.f;
	Engine3D engine;
	auto& window = engine.createWindow(sf::Vector2u(width, width * 9.f / 16.f));

	while (window.isOpen()) {
		engine.handleEvents();

		window.clear(sf::Color(10, 10, 10));
		engine.renderPotentialField(potentialDist);
		engine.renderMasses();
		window.display();
	}

	return 0;
}
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

	using namespace std::chrono;
	size_t totTime = 0;
	size_t fieldTime = 0;
	size_t stabTime = 0;
	size_t idleTime = 0;

	size_t frame = 0;
	while (window.isOpen()) {
		auto t0 = system_clock::now();
		int status = engine.handleEvents();
		if (status == 1)
			isRunning = !isRunning;

		//fast for small N
		if (isRunning)
			sim.step(1.0 / fps);

		window.clear(sf::Color(10, 10, 10));
		
		auto t = system_clock::now();
		engine.renderPotentialField(sim);
		fieldTime += duration_cast<microseconds>(system_clock::now() - t).count();

		//extremely fast
		engine.renderBodies(bodies);

		t = system_clock::now();
		if (isRunning)
			sim.calculateStabilityPoints(stabilityPoints);
		stabTime += duration_cast<microseconds>(system_clock::now() - t).count();

		//extremely fast
		engine.renderPoints(stabilityPoints);

		//actual displaying is fast, also handles idling
		t = system_clock::now();
		window.display();
		idleTime += duration_cast<microseconds>(system_clock::now() - t).count();
		totTime += duration_cast<microseconds>(system_clock::now() - t0).count();

		if (++frame % (2 * fps) == 0) {
			std::string actualFps = std::format("{:.2f}", 2 * fps * 1'000'000.f / totTime);
			std::string fieldPer = std::format("{:.2f}", 100 * float(fieldTime) / totTime);
			std::string stabPer = std::format("{:.2f}", 100 * float(stabTime) / totTime);
			std::string idlePer = std::format("{:.2f}", 100 * float(idleTime) / totTime);
			totTime = 0, fieldTime = 0, stabTime = 0, idleTime = 0;
			std::cout << "fps: " << actualFps << " - field: " << fieldPer << 
				"%, stability: " << stabPer << "%, idle: " << idlePer << "%\n";

			//Vec3 momentum(0, 0, 0);
			//for (const auto& b : bodies)
			//	momentum += b.mass * b.velocity;
			//std::cout << "momentum: <" << momentum.x << ", " << momentum.y << ", " << momentum.z << ">\n";
		}
	}

	return 0;
}
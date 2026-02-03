#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "pendulum.hpp"

int main() {
	sf::Shader glowShader;
	if (!glowShader.loadFromFile("glow.frag", sf::Shader::Type::Fragment))
		return -1;
	glowShader.setUniform("fade", 1.f);
	glowShader.setUniform("strength", 2.f);

	Pendulum pendulum(1, 0.8, 1, 1.1);
	const auto arms = pendulum.getArmLengths();
	const auto& state = pendulum.setState({ 2.2, 3.5, 0, 0 });
	Pendulum::State lastState = state;
	bool isPlaying = false;

	const float width = sf::VideoMode::getFullscreenModes()[0].size.x * 2 / 3.f;
	sf::ContextSettings settings;
	settings.antiAliasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(sf::Vector2u(width, width * 9.f / 16.f)), "Double-pendulum", sf::State::Windowed, settings);
	const unsigned int fps = 100;
	bool isFullscreen = false;
	window.setFramerateLimit(fps);
	window.setKeyRepeatEnabled(false);

	sf::Vector2u wSize = window.getSize();
	sf::Vector2u lastWindowSize = wSize;
	window.setView(sf::View(sf::FloatRect({ 0, 0 }, sf::Vector2f(wSize))));
	sf::RenderTexture rtA(wSize), rtB(wSize);
	size_t ping = 0;

	while (window.isOpen()) {
		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>())
				window.close();
			else if (const auto* k = event->getIf<sf::Event::KeyPressed>()) {
				if (k->code == sf::Keyboard::Key::Space)
					isPlaying = !isPlaying;
				else if (k->code == sf::Keyboard::Key::Enter && k->alt) {
					if (!isFullscreen)
						window.create(sf::VideoMode::getFullscreenModes()[0], "Double-pendulum", sf::State::Fullscreen, settings);
					else
						window.create(sf::VideoMode(sf::Vector2u(width, width * 9.f / 16.f)), "Double-pendulum", sf::State::Windowed, settings);

					window.setFramerateLimit(fps);
					window.setKeyRepeatEnabled(false);
					isFullscreen = !isFullscreen;
				}
			}
		}

		wSize = window.getSize();
		if (wSize != lastWindowSize) {
			lastWindowSize = wSize;

			auto _ = rtA.resize(wSize);
			_ = rtB.resize(wSize);
			rtA.clear(sf::Color::Black);
			rtB.clear(sf::Color::Black);
			window.setView(sf::View(sf::FloatRect({ 0, 0 }, sf::Vector2f(wSize))));
		}

		sf::RenderTexture& current = (ping % 2) ? rtA : rtB;
		sf::RenderTexture& previous = (ping % 2) ? rtB : rtA;

		float totalArmLegth = std::min(wSize.x, wSize.y) / 2.f - 20.f;
		float armLength1 = totalArmLegth * arms.first / (arms.first + arms.second);
		float armLength2 = totalArmLegth * arms.second / (arms.first + arms.second);

		sf::Vector2f pos1(
			wSize.x / 2.f + armLength1 * sin(state.theta1),
			wSize.y / 2.f + armLength1 * cos(state.theta1)
		);
		sf::Vector2f pos2(
			pos1.x + armLength2 * sin(state.theta2),
			pos1.y + armLength2 * cos(state.theta2)
		);

		sf::Vector2f pos1Last(
			wSize.x / 2.f + armLength1 * sin(lastState.theta1),
			wSize.y / 2.f + armLength1 * cos(lastState.theta1)
		);
		sf::Vector2f pos2Last(
			pos1Last.x + armLength2 * sin(lastState.theta2),
			pos1Last.y + armLength2 * cos(lastState.theta2)
		);

		sf::CircleShape centerDot(totalArmLegth / 70.f);
		centerDot.setOrigin({ centerDot.getRadius(), centerDot.getRadius() });
		centerDot.setPosition({ wSize.x / 2.f, wSize.y / 2.f });
		centerDot.setFillColor(sf::Color(100, 100, 100));
		sf::CircleShape dot2 = centerDot;
		dot2.setPosition(pos2);
		centerDot.setOutlineThickness(totalArmLegth / 100.f);
		centerDot.setOutlineColor(sf::Color(20, 20, 20));		
		sf::CircleShape dot1 = centerDot;
		dot1.setPosition(pos1);
		dot2.setFillColor(sf::Color(100, 250, 250));

		sf::RectangleShape arm1(sf::Vector2f(armLength1, totalArmLegth / 50.f));
		arm1.setOrigin({ 0, arm1.getSize().y / 2.f });
		arm1.setPosition({ wSize.x / 2.f, wSize.y / 2.f });
		arm1.setRotation(sf::radians(3.14159 / 2 - state.theta1));
		arm1.setFillColor(sf::Color(20, 20, 20));
		sf::RectangleShape arm2 = arm1;
		arm2.setSize({ armLength2, arm2.getSize().y });
		arm2.setPosition(pos1);
		arm2.setRotation(sf::degrees(90) - sf::radians(state.theta2));

		glowShader.setUniform("radius", dot2.getRadius() / 1.1f);
		glowShader.setUniform("previousFrame", previous.getTexture());
		glowShader.setUniform("pos1", sf::Vector2f(pos2Last.x, wSize.y - pos2Last.y));
		glowShader.setUniform("pos2", sf::Vector2f(pos2.x, wSize.y - pos2.y));
		glowShader.setUniform("resolution", sf::Vector2f(wSize));
		glowShader.setUniform("isPlaying", isPlaying);

		sf::RectangleShape quad(sf::Vector2f{ wSize });
		current.clear(sf::Color::Black);
		current.draw(quad, &glowShader);
		current.display();

		window.clear();
		window.draw(sf::Sprite(current.getTexture()));

		window.draw(arm1);
		window.draw(arm2);
		window.draw(centerDot);
		window.draw(dot1);
		window.draw(dot2);

		window.display();
		ping++;

		lastState = state;
		//print total energy and compute next step
		if (isPlaying) {
			auto u = pendulum.getPotentialEnergy();
			auto k = pendulum.getKineticEnergy();
			if (ping % (fps / 2) == 0)
				std::cout << "Total energy: " << u + k << " (U: " << u << ", K: " << k << ")\n";
			pendulum.step(1.0 / fps);
		}
		
	}
	return 0;

}
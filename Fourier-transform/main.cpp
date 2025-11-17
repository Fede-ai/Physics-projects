#include <iostream>
#include <SFML/Graphics.hpp>
#include "transform.hpp"

int main()
{
	const float width = sf::VideoMode::getFullscreenModes()[0].size.x * 2 / 3.f;
	sf::Vector2f wSize(width, width * 9.f / 16.f);
	sf::ContextSettings settings;
	settings.antiAliasingLevel = 8;

	sf::RenderWindow w(sf::VideoMode(sf::Vector2u(wSize)), "FFT", sf::State::Windowed, settings);
	w.setView(sf::View({ 0, 0 }, wSize));
	w.setKeyRepeatEnabled(false);
	w.setFramerateLimit(100);

	sf::Vector2f lastMousePos;
	bool isMoving = false, isRunning = false;
	bool isDrawing = false;
	float zoom = 1.0;
	size_t drawingFrame = size_t(-1);

	std::vector<Point> points;
	std::vector<Point> uniform;
	Transform ft;

	sf::VertexArray line(sf::PrimitiveType::LineStrip);
	sf::VertexArray traced(sf::PrimitiveType::LineStrip);
	
	sf::Font font("font.ttf");

	while (w.isOpen()) {
		while (const std::optional event = w.pollEvent()) {
			if (event->is<sf::Event::Closed>())
				w.close();
			else if (const auto* button = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (button->button == sf::Mouse::Button::Left) {
					if (points.empty())
						isDrawing = true;
					else
						isMoving = true;
					lastMousePos = w.mapPixelToCoords(button->position);
				}
			}
			else if (const auto* button = event->getIf<sf::Event::MouseButtonReleased>()) {
				if (button->button == sf::Mouse::Button::Left) {
					if (isDrawing && !points.empty()) {
						isRunning = true;

						uniform = ft.smoothenPoints(points);
						ft.performDFT(uniform);
						for (int i = 0; i < ft.spectrum_.size(); i++)
							std::cout << "f " << i << " -> mag: " << std::abs(ft.spectrum_[i]) / ft.spectrum_.size() << "\tphase: " << std::arg(ft.spectrum_[i]) << "\n";

						ft.orderSpectrum();
					}
					isDrawing = false, isMoving = false;
				}
			}
			else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>()) {
				if (isDrawing) {
					auto p = w.mapPixelToCoords(mouse->position);
					points.push_back(std::complex<double>(p.x, p.y));
					line.append(sf::Vertex(p, sf::Color::White));
				}
				else if (isMoving) {
					sf::View v = w.getView();
					v.move(lastMousePos - w.mapPixelToCoords(mouse->position));
					w.setView(v);

					lastMousePos = w.mapPixelToCoords(mouse->position);
				}
			}
			else if (const auto* mouse = event->getIf<sf::Event::MouseWheelScrolled>())
				zoom /= ((mouse->delta > 0) ? 0.85f : 1.15f);
			else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
				if (key->code == sf::Keyboard::Key::Space && !isDrawing && !points.empty())
					isRunning = !isRunning;
		}

		wSize = sf::Vector2f(w.getSize());
		sf::View v = w.getView();
		v.setSize(wSize / zoom);
		w.setView(v);
		w.clear(sf::Color(50, 50, 50));

		//draw FT result
		sf::VertexArray arms(sf::PrimitiveType::LineStrip);
		const float speed = 150 / 1'000.f;
		if (drawingFrame != size_t(-1) || isRunning) {
			if (isRunning)
				drawingFrame++;

			float N = ft.spectrum_.size();
			float t = speed * drawingFrame / N;
			float x = 0, y = 0;

			for (const auto& f : ft.orderedSpectrum_) {
				float angle = 2 * PI * f.m * t + f.phase;
				x += f.magnitude * std::cos(angle);
				y += f.magnitude * std::sin(angle);

				arms.append(sf::Vertex(sf::Vector2f(x, y), sf::Color(100, 100, 255)));
			}
			if (isRunning && (speed * drawingFrame <= ft.spectrum_.size()))
				traced.append(sf::Vertex(sf::Vector2f(x, y), sf::Color::Red));
		}

		w.draw(line);
		w.draw(arms);
		w.draw(traced);

		sf::RenderTexture txt(sf::Vector2u{ wSize });
		txt.clear(sf::Color::Transparent);

		float u = txt.getSize().y * 0.01f;
		sf::RectangleShape menuBg;
		menuBg.setSize(sf::Vector2f(txt.getSize().x, 10 * u));
		menuBg.setFillColor(sf::Color(20, 20, 20));
		txt.draw(menuBg);

		sf::Text counter(font, "Raw points n. = " + std::to_string(points.size()) +
			"\nFrequencies n. = " + std::to_string(ft.spectrum_.size()));
		counter.setPosition({ 1.3f * u, 0.4f * u });
		counter.setCharacterSize(3.6f * u);
		txt.draw(counter);

		sf::Sprite sprite(txt.getTexture());
		sprite.setScale({ 1.f / zoom, -1.f / zoom });
		sprite.setPosition(w.mapPixelToCoords(sf::Vector2i(0, wSize.y)));
		w.draw(sprite);

		w.display();
	}

	return 0;
}
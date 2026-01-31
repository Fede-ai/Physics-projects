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
	float zoom = 1.0, threshold = 0.05f;
	size_t numFrequencies = 0;
	size_t drawingFrame = size_t(-1);

	std::vector<std::vector<Point>> points;
	Transform* ft = new Transform({});

	std::vector<sf::VertexArray> lines;
	sf::VertexArray traced(sf::PrimitiveType::LineStrip);
	
	sf::Font font("font.ttf");

	while (w.isOpen()) {
		while (const std::optional event = w.pollEvent()) {
			if (event->is<sf::Event::Closed>())
				w.close();
			else if (const auto* button = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (button->button == sf::Mouse::Button::Left) {
					if (drawingFrame == size_t(-1)) {
						lines.push_back(sf::VertexArray(sf::PrimitiveType::LineStrip));
						points.push_back({});

						isDrawing = true;
					}
					else
						isMoving = true;
					lastMousePos = w.mapPixelToCoords(button->position);
				}
			}
			else if (const auto* button = event->getIf<sf::Event::MouseButtonReleased>()) {
				if (button->button == sf::Mouse::Button::Left) {
					if (isDrawing && points[points.size() - 1].size() < 20) {
						lines.pop_back();
						points.pop_back();
					}
					isDrawing = false, isMoving = false;
				}
			}
			else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>()) {
				if (isDrawing) {
					auto p = w.mapPixelToCoords(mouse->position);
					points[points.size() - 1].push_back({ p.x, p.y });
					lines[lines.size() - 1].append(sf::Vertex(p, sf::Color::White));
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
			else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
				if (key->code == sf::Keyboard::Key::Space) {
					if (drawingFrame == size_t(-1) && !isDrawing && !points.empty()) {
						isRunning = true;

						std::vector<Point> continuous;
						for (auto& p : points) {
							for (auto& pt : p)
								continuous.push_back(pt);
						}

						auto uniform = Transform::smoothenPoints(continuous);
						ft = new Transform(uniform);
						for (int i = 0; i < ft->spectrum_.size(); i++)
							std::cout << "f " << i << " -> mag: " << std::abs(ft->spectrum_[i]) / ft->spectrum_.size()
							<< "\tphase: " << std::arg(ft->spectrum_[i]) << "\n";

						std::cout << "raw points number: " << continuous.size() << "\n";
						std::cout << "uniform points number: " << uniform.size() << "\n";

						for (size_t i = 0; i < ft->orderedSpectrum_.size(); i++) {
							if (ft->orderedSpectrum_[i].magnitude >= threshold)
								numFrequencies++;
						}
						std::cout << "frequencies with magnitude > " << threshold << ": " << numFrequencies << "\n";
					}
					else if (drawingFrame != size_t(-1))
						isRunning = !isRunning;
				}
				else if (key->code == sf::Keyboard::Key::R && !isDrawing) {
					isRunning = false;
					drawingFrame = size_t(-1);
					numFrequencies = 0;

					points.clear();
					lines.clear();
					traced.clear();

					delete ft;
					ft = new Transform({});
				}
			}
		}

		wSize = sf::Vector2f(w.getSize());
		sf::View v = w.getView();
		v.setSize(wSize / zoom);
		w.setView(v);
		w.clear(sf::Color(50, 50, 50));		
		
		//draw FT result
		const float speed = 150 / 1'000.f;
		if (drawingFrame != size_t(-1) || isRunning) {
			if (isRunning)
				drawingFrame++;

			float t = speed * drawingFrame / ft->orderedSpectrum_.size();
			const auto& anchor = ft->orderedSpectrum_[0];
			float x = anchor.magnitude * std::cos(anchor.phase);
			float y = anchor.magnitude * std::sin(anchor.phase);

			for (const auto& f : ft->orderedSpectrum_) {
				if (f.magnitude < threshold || f.m == 0)
					continue;

				float radius = std::pow(f.magnitude, 0.5f) / 5.f - 0.04f;
				float c = std::pow(f.magnitude / ft->orderedSpectrum_[1].magnitude, 0.4);
				sf::Color color = sf::Color(0, 255 * c, 255 * (1 - c));

				sf::CircleShape circle(radius);
				circle.setOrigin({ radius, radius });
				circle.setPosition({ x, y });
				circle.setFillColor(color);
				w.draw(circle);

				float angle = 2 * PI * f.m * t + f.phase;
				sf::RectangleShape body({ float(f.magnitude), 2.f * radius });
				body.setOrigin({ 0, radius });
				body.setRotation(sf::radians(angle));
				body.setPosition({ x, y });
				body.setFillColor(color);
				w.draw(body);

				x += f.magnitude * std::cos(angle);
				y += f.magnitude * std::sin(angle);

				circle.setPosition({ x, y });
				w.draw(circle);
			}

			if (isRunning && (speed * drawingFrame <= ft->orderedSpectrum_.size()))
				traced.append(sf::Vertex(sf::Vector2f(x, y), sf::Color::Red));
		}

		for (auto& line : lines)
			w.draw(line);
		w.draw(traced);

		sf::RenderTexture txt(sf::Vector2u{ wSize });
		txt.clear(sf::Color::Transparent);

		float u = txt.getSize().y * 0.01f;
		sf::RectangleShape menuBg;
		menuBg.setSize(sf::Vector2f(txt.getSize().x, 10 * u));
		menuBg.setFillColor(sf::Color(20, 20, 20));
		txt.draw(menuBg);

		size_t nPoints = 0;
		for (auto& p : points)
			nPoints += p.size();

		sf::Text counter(font, "Raw points n. = " + std::to_string(nPoints) +
			"\nFrequencies n. = " + std::to_string(numFrequencies));
		counter.setPosition({ 1.3f * u, 0.4f * u });
		counter.setCharacterSize(3.6f * u);
		txt.draw(counter);

		sf::Sprite sprite(txt.getTexture());
		sprite.setScale({ 1.f / zoom, -1.f / zoom });
		sprite.setPosition(w.mapPixelToCoords(sf::Vector2i(0, wSize.y)));
		w.draw(sprite);

		w.display();
	}

	if (ft != nullptr)
		delete ft;

	return 0;
}
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

	bool isDrawing = false;
	size_t drawingFrame = 0;

	List points;
	Transform ft;
	sf::VertexArray line(sf::PrimitiveType::LineStrip);
	sf::VertexArray traced(sf::PrimitiveType::LineStrip);

	while (w.isOpen()) {
		while (const std::optional event = w.pollEvent()) {
			if (event->is<sf::Event::Closed>())
				w.close();
			else if (event->is<sf::Event::Resized>()) {
				wSize = sf::Vector2f(w.getSize());
				w.setView(sf::View({ 0, 0 }, wSize));
			}
			else if (const auto* button = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (button->button == sf::Mouse::Button::Left && points.empty())
					isDrawing = true;
			}
			else if (const auto* button = event->getIf<sf::Event::MouseButtonReleased>()) {
				if (button->button == sf::Mouse::Button::Left && isDrawing && !points.empty()) {
					isDrawing = false;
					drawingFrame = 1;

					ft.performDFT(points);
					for (int i = 0; i < ft.spectrum_.size(); i++) {
						std::cout << "Frequency " << i << " -> Magnitude: " << std::abs(ft.spectrum_[i]) / ft.spectrum_.size() << " Phase: " << std::arg(ft.spectrum_[i]) << "\n";
					}

					//auto reconstructed = ft.performIDFT();
					//for (auto& p : reconstructed) {
					//	traced.append(sf::Vertex(sf::Vector2f(p.real(), p.imag()), sf::Color::Red));
					//}
				}
			}
			else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>()) {
				if (isDrawing) {
					auto p = w.mapPixelToCoords(mouse->position);
					points.push_back(std::complex<double>(p.x, p.y));
					line.append(sf::Vertex(p, sf::Color::White));
				}
			}
		}

		w.clear(sf::Color(20, 20, 20));

		//draw FFT result
		if (drawingFrame > 0) {
			float N = ft.spectrum_.size();
			float t = (drawingFrame - 1) / (25.f * N);
			float x = 0, y = 0;

			for (int i = 0; i < ft.spectrum_.size(); i++) {
				auto magnitude = std::abs(ft.spectrum_[i]) / N;
				auto phase = std::arg(ft.spectrum_[i]);

				int m = (i <= N / 2) ? i : (i - N);
				float angle = 2 * PI * m * t + phase;

				x += magnitude * std::cos(angle);
				y += magnitude * std::sin(angle);
			}
			traced.append(sf::Vertex(sf::Vector2f(x, y), sf::Color::Red));
			drawingFrame++;
		}

		w.draw(line);
		w.draw(traced);
		w.display();
	}

	return 0;
}
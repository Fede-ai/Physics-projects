#include <iostream>
#include "pendulum.hpp"

int main() {
	Pendulum pendulum;
	const float width = sf::VideoMode::getFullscreenModes()[0].size.x * 2 / 3.f;
	sf::Vector2f wSize(width, width * 9.f / 16.f);

	sf::ContextSettings settings;
	settings.antiAliasingLevel = 8;

	sf::RenderWindow w(sf::VideoMode(sf::Vector2u(wSize)), "Pendulum", sf::State::Windowed, settings);
	w.setView(sf::View({ wSize.x / 2.f, wSize.y / 2.f }, { wSize.x, wSize.y }));
	w.setKeyRepeatEnabled(false);
	w.setFramerateLimit(50);

	sf::RectangleShape body;
	body.setOrigin({ 0, 10 });
	body.setFillColor(sf::Color(50, 50, 50));

	sf::CircleShape pivot(6);
	pivot.setOrigin({ 6, 6 });
	pivot.setOutlineColor(sf::Color(50, 50, 50));
	pivot.setOutlineThickness(4);
	pivot.setFillColor(sf::Color::Black);

	sf::CircleShape mass(15);
	mass.setOrigin({ 15, 15 });
	mass.setFillColor(sf::Color(200, 10, 10));

	sf::VertexArray arrow(sf::PrimitiveType::TriangleFan, 9);
	for (int i = 0; i < 9; i++)
		arrow[i].color = sf::Color(150, 150, 150);

	sf::Font font("font.ttf");
	sf::Text eq(font, "", 25);
	sf::Text titles(font, "", 25);
	titles.setPosition({ 15, 8 });

	while (w.isOpen()) {
		while (const std::optional event = w.pollEvent()) {
			if (event->is<sf::Event::Closed>())
				w.close();
			else if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>())
				pendulum.applyZoom(wheel->delta);
			else if (event->is<sf::Event::Resized>()) {
				wSize = sf::Vector2f(w.getSize());
				w.setView(sf::View({ wSize.x / 2.f, wSize.y / 2.f }, { wSize.x, wSize.y }));
				pendulum.forceRerender();
			}
			//toogle fullscreen
			else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
				if (key->code == sf::Keyboard::Key::Space)
					pendulum.isUpdating = !pendulum.isUpdating;
				else if (key->code == sf::Keyboard::Key::Enter && key->alt) {
					if (w.getSize().x != sf::VideoMode::getFullscreenModes()[0].size.x)
						w.create(sf::VideoMode::getFullscreenModes()[0], "Pendulum", sf::State::Fullscreen, settings);
					else
						w.create(sf::VideoMode(sf::Vector2u(wSize)), "Pendulum", sf::State::Windowed, settings);

					sf::Vector2f newSize = sf::Vector2f(w.getSize());
					w.setView(sf::View({ newSize.x / 2.f, newSize.y / 2.f }, { newSize.x, newSize.y }));
					w.setKeyRepeatEnabled(false);
					w.setFramerateLimit(50);

					pendulum.forceRerender();
				}
			}
			//start moving graph
			else if (const auto* button = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (button->button == sf::Mouse::Button::Left)
					pendulum.isMovingGraph = true;
				else if (button->button == sf::Mouse::Button::Right && button->position.x > w.getSize().y * 0.8)
					pendulum.setPendulum(button->position.x - int(w.getSize().y * 0.8), button->position.y,
						int(w.getSize().x - w.getSize().y * 0.8), w.getSize().y);
			}
			//stop moving graph
			else if (const auto* button = event->getIf<sf::Event::MouseButtonReleased>()) {
				if (button->button == sf::Mouse::Button::Left)
					pendulum.isMovingGraph = false;
			}
		}

		//modify drag
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
			pendulum.drag += 5;
			pendulum.forceRerender();
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
			pendulum.drag -= 5;
			pendulum.forceRerender();
		}

		if (pendulum.isUpdating)
			pendulum.update();

		w.clear(sf::Color(20, 20, 20));

		//render graph if needed
		int graphWidth = int(w.getSize().x - w.getSize().y * 0.8);
		if (graphWidth > 0) {
			pendulum.moveGraph();

			sf::Texture texture(pendulum.renderGraph(graphWidth, w.getSize().y));
			sf::Sprite graph(texture);
			graph.setPosition({ float(w.getSize().x - graphWidth), 0 });
			w.draw(graph);
		}

		float xPos = float(w.getSize().x - std::max(graphWidth, 0)) / 2.f;
		float ang = float(0.5 - pendulum.pendulum.x);

		body.setSize(sf::Vector2f(xPos - 50, 20));
		body.setPosition({ xPos, w.getSize().y / 2.f });
		body.setRotation(sf::degrees(ang * 180));
		w.draw(body);
		
		pivot.setPosition({ xPos, w.getSize().y / 2.f });
		w.draw(pivot);

		mass.setPosition(pivot.getPosition());
		mass.move({ body.getSize().x * float(cos(ang * Pendulum::pi)),
			body.getSize().x * float(sin(ang * Pendulum::pi)) });
		w.draw(mass);

		float arrowLeng = float(pendulum.pendulum.y * 50);
		sf::Vector2f offset = sf::Vector2f(4 * float(cos(ang * Pendulum::pi)), 
			4 * float(sin(ang * Pendulum::pi)));
		arrow[0].position = mass.getPosition() + sf::Vector2f(arrowLeng * float(cos((ang - 0.5f)
			* Pendulum::pi)), arrowLeng * float(sin((ang - 0.5f) * Pendulum::pi)));
		arrow[1].position = mass.getPosition() + sf::Vector2f(1.2f * arrowLeng * float(cos((ang - 0.5f) 
			* Pendulum::pi)), 1.2f * arrowLeng * float(sin((ang - 0.5f) * Pendulum::pi)));
		arrow[2].position = arrow[0].position + (offset + offset);
		arrow[3].position = arrow[0].position + offset;
		arrow[4].position = mass.getPosition() + offset;
		arrow[5].position = mass.getPosition() - offset;
		arrow[6].position = arrow[0].position - offset;
		arrow[7].position = arrow[0].position - (offset + offset);
		arrow[8].position = arrow[1].position;
		w.draw(arrow);

		titles.setString("Ang = " + std::to_string(pendulum.pendulum.x * Pendulum::pi) + " rad\nAng vel = "
			+ std::to_string(pendulum.pendulum.y) + " rad / sec");
		w.draw(titles);

		std::string uStr = std::to_string(pendulum.drag / 1000.f);
		while (uStr[uStr.size() - 1] == '0')
			uStr.resize(uStr.size() - 1);
		eq.setString("u = " + uStr + ", a = a(t)\na'' = -u * a' - (g / L) * sin(a)");
		eq.setPosition({ 15, float(w.getSize().y - 75) });
		w.draw(eq);

		w.display();
	}

	return 0;
}
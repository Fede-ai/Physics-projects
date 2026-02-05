#include "engine.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

Engine3D::Engine3D(unsigned int fps)
	:
	fps_(fps)
{
	camera_.position = Vec3(-310, -170, -170);
	camera_.fov = 60 * PI / 180.0;
	camera_.pitch = 30 * PI / 180.0;
	camera_.yaw = 60 * PI / 180.0;

	settings_.antiAliasingLevel = 8;
	f = 1.0f / tan(camera_.fov * 0.5f);
}

sf::RenderWindow& Engine3D::createWindow(sf::Vector2u size)
{
	window_.create(sf::VideoMode(size), "Gravitational-potential", sf::State::Windowed, settings_);
	window_.setFramerateLimit(fps_);
	window_.setKeyRepeatEnabled(false);
	window_.setView(sf::View(sf::FloatRect(-sf::Vector2f(size) / 2.f, sf::Vector2f(size))));

	defaultWindowSize_ = size;
	lastMousePos_ = sf::Mouse::getPosition(window_);

	return window_;
}

int Engine3D::handleEvents()
{	
	//positive x when yaw = 0
	Vec3 right = {
		cos(camera_.yaw),
		0,
		-sin(camera_.yaw)
	};
	//positive y when pitch = 0
	Vec3 down = {
		sin(-camera_.pitch) * sin(camera_.yaw),
		cos(camera_.pitch),
		sin(-camera_.pitch) * cos(camera_.yaw)
	};	
	// (right X down) positive z when pitch = 0 and yaw = 0
	Vec3 forward = right.cross(down);

	int status = 0;
    while (const std::optional event = window_.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window_.close();
		else if (const auto* w = event->getIf<sf::Event::MouseWheelScrolled>())
			camera_.position += forward * (10.0 * w->delta);
		else if (const auto* m = event->getIf<sf::Event::MouseMoved>()) {
			auto delta = m->position - lastMousePos_;
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
				camera_.yaw -= 0.0015 * delta.x;
				camera_.pitch -= 0.0015 * delta.y;
				camera_.pitch = std::clamp(camera_.pitch, -PI / 2 + 0.01, PI / 2 - 0.01);
			}
			else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
				camera_.position -= right * (0.3 * delta.x);
				camera_.position -= down * (0.3 * delta.y);
			}
		}
		else if (const auto* k = event->getIf<sf::Event::KeyPressed>()) {
			//toggle pause
			if (k->code == sf::Keyboard::Key::Space)
				status = 1;
			else if (k->code == sf::Keyboard::Key::Enter && k->alt) {
				if (!isFullscreen_)
					window_.create(sf::VideoMode::getFullscreenModes()[0], "Gravitational-potential", sf::State::Fullscreen, settings_);
				else
					window_.create(sf::VideoMode(defaultWindowSize_), "Gravitational-potential", sf::State::Windowed, settings_);

				window_.setFramerateLimit(fps_);
				window_.setKeyRepeatEnabled(false);
				isFullscreen_ = !isFullscreen_;
				lastMousePos_ = sf::Mouse::getPosition(window_);
			}
		}
    }
	lastMousePos_ = sf::Mouse::getPosition(window_);
	sf::Vector2u size = window_.getSize();
	window_.setView(sf::View(sf::FloatRect(-sf::Vector2f(size) / 2.f, sf::Vector2f(size))));

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		camera_.fov += 0.01;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		camera_.fov -= 0.01;
	f = 1.0f / tan(camera_.fov * 0.5f) * size.x / 2.f;

	//std::cout << "(" << camera_.position.x << ", " << camera_.position.y << ", " << camera_.position.z << "), ";
	//std::cout << "yaw: " << camera_.yaw * 180 / PI << ", pitch: " << camera_.pitch * 180 / PI << "\n";

	return status;
}

void Engine3D::renderPotentialField(std::function<double(double, double)> dist)
{
	sf::Vector2u windowSize = window_.getSize();
	auto applyPerspective = [&](Vec3 p) -> sf::Vector2f {
		float px = p.x * f / p.z;
		float py = p.y * f / p.z;
		return sf::Vector2f(px, py);
		};

	auto addLine = [&](Vec3 p0, Vec3 p1) {

		int steps = 1000;
		sf::VertexArray lineArray(sf::PrimitiveType::LineStrip);
		for (int i = 0; i <= steps; i++) {
			double t = float(i) / steps;
			Vec3 p = p0 * (1.0f - t) + p1 * t;

			auto pc = transformToCameraSpace(p + Vec3(0, dist(p.x, p.z), 0));
			if (pc.z <= 0)
				continue;
			lineArray.append({ applyPerspective(pc) });
		}
		window_.draw(lineArray);
		};

	//from -1000 to 1000 in x and z, step 100
	int s = 200, step = 10;
	for (int x = -s; x <= s; x += step)
		addLine(Vec3(x, 0, -s), Vec3(x, 0, s));
	for (int z = -s; z <= s; z += step)
		addLine(Vec3(-s, 0, z), Vec3(s, 0, z));
}

void Engine3D::renderBodies(std::vector<Body>& bodies)
{
	std::vector<std::pair<Body*, Vec3>> sorted;
	sorted.reserve(bodies.size());
	for (auto& b : bodies) {
		auto relPos = transformToCameraSpace(b.position);
		if (relPos.z > 0)
			sorted.push_back({ &b, relPos });
	}

	std::sort(sorted.begin(), sorted.end(),
		[](std::pair<Body*, Vec3> a, std::pair<Body*, Vec3> b) {
			return a.second.z > b.second.z;
		});

	//draw bodies with mass
	for (const auto& b : sorted) {
		float cx = b.second.x * f / b.second.z;
		float cy = b.second.y * f / b.second.z;
		float screenRadius = b.first->radius * f / b.second.z;

		sf::CircleShape circle;
		circle.setRadius(screenRadius);
		circle.setOrigin({ screenRadius, screenRadius });
		circle.setPosition({ cx, cy });

		//differenciate massless bodies
		if (b.first->mass > 0)
			circle.setFillColor(sf::Color::Red);
		else
			circle.setFillColor(sf::Color::Blue);

		window_.draw(circle);
	}
}

Vec3 Engine3D::transformToCameraSpace(const Vec3& pos) const
{
	auto applyJaw = [](const Vec3& v, double yaw) -> Vec3 {
		double cosYaw = cos(yaw), sinYaw = sin(yaw);
		return Vec3{
			v.x * cosYaw + v.z * sinYaw,
			v.y,
			v.z * cosYaw - v.x * sinYaw
		};
		};
	auto applyPitch = [](const Vec3& v, double pitch) -> Vec3 {
		double cosPitch = cos(pitch), sinPitch = sin(pitch);
		return Vec3{
			v.x,
			v.y * cosPitch + v.z * sinPitch,
			v.z * cosPitch - v.y * sinPitch
		};
		};

	Vec3 relPos = pos - camera_.position;
	relPos = applyJaw(relPos, -camera_.yaw);
	relPos = applyPitch(relPos, -camera_.pitch);

	return relPos;
}

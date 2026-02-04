#include "engine.hpp"
#include <iostream>

Engine3D::Engine3D()
{
	camera_.position = Vec3(-130, -130, -130);
	camera_.fov = 60.0 * PI / 180.0;
	camera_.pitch = 30 * PI / 180.0;
	camera_.yaw = 45 * PI / 180.0;
}

sf::RenderWindow& Engine3D::createWindow(sf::Vector2u size)
{
	sf::ContextSettings settings;
	settings.antiAliasingLevel = 8;

	window_.create(sf::VideoMode(size), "Gravitational-potential", sf::State::Windowed, settings);
	window_.setFramerateLimit(60);
	window_.setKeyRepeatEnabled(false);
	window_.setView(sf::View(sf::FloatRect(-sf::Vector2f(size) / 2.f, sf::Vector2f(size))));

	lastMousePos_ = sf::Mouse::getPosition(window_);

	return window_;
}

void Engine3D::handleEvents()
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
    }
	lastMousePos_ = sf::Mouse::getPosition(window_);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		camera_.fov += 0.01;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		camera_.fov -= 0.01;

	//std::cout << "(" << camera_.position.x << ", " << camera_.position.y << ", " << camera_.position.z << "), ";
	//std::cout << "yaw: " << camera_.yaw * 180 / PI << ", pitch: " << camera_.pitch * 180 / PI << "\n";
}

void Engine3D::renderPotentialField(std::function<double(double, double)> dist)
{
	std::vector<Vec3> circlePos{ {10, -10, -50.0}, {50, -20, 50} };
	double radius = 5.0;
	sf::Vector2u windowSize = window_.getSize();
	float f = 1.0f / tan(camera_.fov * 0.5f);

	auto applyPerspective = [&](Vec3 p) -> sf::Vector2f {
		float px = ((p.x * f) / p.z) * windowSize.x / 2.f;
		float py = ((p.y * f) / p.z) * windowSize.y / 2.f;
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

	for (const auto& pos : circlePos) {
		auto relPos = transformToCameraSpace(pos);

		if (relPos.z <= 0)
			return;

		float cx = ((relPos.x * f) / relPos.z) * windowSize.x / 2.f;
		float cy = ((relPos.y * f) / relPos.z) * windowSize.y / 2.f;
		float screenRadius = (radius * f / relPos.z) * windowSize.y / 2.f;

		sf::CircleShape circle;
		circle.setRadius(screenRadius);
		circle.setOrigin({ screenRadius, screenRadius });
		circle.setPosition({ cx, cy });
		circle.setFillColor(sf::Color::Red);
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

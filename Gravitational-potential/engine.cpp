#include "engine.hpp"
#include <iostream>

Engine3D::Engine3D()
{
	camera_.fov = 60.0 * PI / 180.0;
	camera_.pitch = 0 * PI / 180.0;
	camera_.yaw = 0 * PI / 180.0;
}

sf::RenderWindow& Engine3D::createWindow(sf::Vector2u size)
{
	window_.create(sf::VideoMode(size), "Gravitational-potential");
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
			if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				continue;
			camera_.position -= right * (0.2 * (m->position.x - lastMousePos_.x));
			camera_.position -= down  * (0.2 * (m->position.y - lastMousePos_.y));
		}
    }
	lastMousePos_ = sf::Mouse::getPosition(window_);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		camera_.pitch += 0.005;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		camera_.pitch -= 0.005;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		camera_.yaw += 0.005;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		camera_.yaw -= 0.005;

	//std::cout << "(" << camera_.position.x << ", " << camera_.position.y << ", " << camera_.position.z << "), ";
	//std::cout << "yaw: " << camera_.yaw * 180 / PI << ", pitch: " << camera_.pitch * 180 / PI << "\n";
}

void Engine3D::renderPotentialField(std::function<double(double, double)> dist)
{
	std::vector<Vec3> circlePos{ {10, 10, 100.0}, {50, 10, 200} };
	double radius = 5.0;
	sf::Vector2u windowSize = window_.getSize();

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

	for (const auto& pos : circlePos) {
		Vec3 relPos = pos - camera_.position;
		relPos = applyJaw(relPos, -camera_.yaw);
		relPos = applyPitch(relPos, -camera_.pitch);	
		
		if (relPos.z <= 0)
			return;	
		
		float f = 1.0f / tan(camera_.fov * 0.5f);
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

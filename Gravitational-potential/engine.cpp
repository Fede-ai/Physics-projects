#include "engine.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

Engine3D::Engine3D(unsigned int fps)
	:
	fps_(fps)
{
	lightDirection_ = Vec3(1, -1, 1);
	auto _ = sphereShader_.loadFromFile("sphere.frag", sf::Shader::Type::Fragment);

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
			camera_.position += forward * (15.0 * w->delta);
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

void Engine3D::renderPotentialField(GravitySimulator& sim)
{
	sf::Vector2u windowSize = window_.getSize();
	auto applyPerspective = [&](Vec3 p) -> sf::Vector2f {
		float px = p.x * f / p.z;
		float py = p.y * f / p.z;
		return sf::Vector2f(px, py);
		};

	auto addLine = [&](Vec3 p0, Vec3 p1, sf::Color c = sf::Color::White) {
		int steps = 1000;
		sf::VertexArray lineArray(sf::PrimitiveType::LineStrip);
		for (int i = 0; i <= steps; i++) {
			double t = float(i) / steps;
			Vec3 p = p0 * (1.0f - t) + p1 * t;

			auto pc = transformToCameraSpace(p + Vec3(0, sim.getPotentialAtPoint(p.x, p.z), 0));
			if (pc.z <= 0)
				continue;

			sf::Vertex v;
			v.position = applyPerspective(pc);
			v.color = c;
			lineArray.append(v);
		}
		window_.draw(lineArray);
		};

	//the x axis is drawn cyan
	//the z axis is drawn yellow

	//from -1000 to 1000 in x and z, step 100
	int s = 200, step = 10;
	for (int i = -s; i <= s; i += step) {
		if (i == -200) {
			addLine(Vec3(-s, 0, i), Vec3(s, 0, i), sf::Color(0, 200, 200));
			addLine(Vec3(i, 0, -s), Vec3(i, 0, s), sf::Color(200, 200, 0));
			continue;
		}

		addLine(Vec3(-s, 0, i), Vec3(s, 0, i));
		addLine(Vec3(i, 0, -s), Vec3(i, 0, s));
	}
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

	double cosYaw = cos(camera_.yaw), sinYaw = sin(camera_.yaw);
	double cosPitch = cos(camera_.pitch), sinPitch = sin(camera_.pitch);
	auto ld = Vec3{
		lightDirection_.x * cosYaw + lightDirection_.z * sinYaw,
		lightDirection_.y,
		lightDirection_.z * cosYaw - lightDirection_.x * sinYaw
	};
	ld = Vec3{
		ld.x,
		-(ld.y * cosPitch + ld.z * sinPitch),
		ld.z * cosPitch - ld.y * sinPitch
	}.normalized();

	sf::Vector2f center = sf::Vector2f(window_.getSize()) / 2.f;
	//draw bodies with mass
	for (const auto& b : sorted) {
		float cx = b.second.x * f / b.second.z;
		float cy = b.second.y * f / b.second.z;
		float screenRadius = b.first->radius * f / b.second.z;

		sf::CircleShape circle(screenRadius, std::max(20, int(b.first->radius * 8)));
		circle.setOrigin({ screenRadius, screenRadius });
		circle.setPosition({ cx, cy });

		sphereShader_.setUniform("lightDir", sf::Vector3f(ld));
		sphereShader_.setUniform("radius", screenRadius);
		sphereShader_.setUniform("pos", sf::Vector2f(cx, -cy) + center);
		
		//differenciate massless bodies
		if (b.first->mass > 0)
			sphereShader_.setUniform("baseColor", sf::Glsl::Vec3(1.f, 0.f, 0.f));
		else
			sphereShader_.setUniform("baseColor", sf::Glsl::Vec3(0.f, 0.f, 1.f));
		window_.draw(circle, &sphereShader_);
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

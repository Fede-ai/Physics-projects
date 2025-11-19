#include <iostream>
#include "foil.hpp"
#include "lbm.hpp"

sf::Image generateImg(LBM& lbm, bool drawSolid) {
	sf::Image img(sf::Vector2u(NX, NY));
	bool vorticity = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

	for (unsigned y = 0; y < NY; y++) {
		for (unsigned x = 0; x < NX; x++) {
			int id = x + y * NX;
			if (lbm.is_solid[id]) {
				if (drawSolid)
					img.setPixel({x, y}, {0, 20, 20});
				continue;
			}
    		double speed = sqrt(lbm.ux[id] * lbm.ux[id] + lbm.uy[id] * lbm.uy[id]);
    		double s = std::min(speed / 0.2, 1.0);

			if (vorticity) {
				double dudy = (lbm.uy[x + 1 + y * NX] - lbm.uy[x - 1 + y * NX]) * 0.5;
    			double dudx = (lbm.ux[x + (y + 1) * NX] - lbm.ux[x + (y - 1) * NX]) * 0.5;
				s = std::min((dudy - dudx) * 45, 1.0);
			}
			
			s = std::min(std::max(s, -1.0), 1.0);
    		uint8_t r = 255.f * std::max(s, 0.0);
    		uint8_t g = 255.f * (1.0 - abs(s));
			uint8_t b = 255.f * std::max(-s, 0.0);
			img.setPixel({x, y}, {r, g, b});
  		}
	}

	return img;
}

void buildSolid(Foil& f, LBM& lbm, float chord) {
	sf::ContextSettings settings;
	settings.antiAliasingLevel = 8;
	sf::RenderTexture rt({NX, NY}, settings);
	rt.setView(sf::View({0, 0}, {NX, NY}));
	rt.clear(sf::Color::Black);

	sf::VertexArray foil(sf::PrimitiveType::TriangleFan);
	sf::Vertex v;
	v.color = sf::Color::White;
	v.position = {0, 0};
	foil.append(v);

	for (size_t i = 0; i < f.upperFoil.getVertexCount(); i++) {
		v.position = {f.upperFoil[i].position.x * chord, f.upperFoil[i].position.y * chord};
		foil.append(v);
	}
	for (size_t i = f.lowerFoil.getVertexCount(); i > 0; i--) {
		v.position = {f.lowerFoil[i - 1].position.x * chord, f.lowerFoil[i - 1].position.y * chord};
		foil.append(v);
	}
	v.position = {f.upperFoil[0].position.x * chord, f.upperFoil[0].position.y * chord};
	foil.append(v);

	rt.draw(foil);
	rt.display();
	auto img = rt.getTexture().copyToImage();
	auto p = img.getPixelsPtr();

	lbm.is_solid.assign(NX * NY, 0);
	for (int x = 0; x < NX; x++) {
		for (int y = 0; y < NY; y++) {
			if (int(p[(x + y * NX) * 4]) > 100)
				lbm.is_solid[x + y * NX] = 1;
		}

		lbm.is_solid[x] = 1;
		lbm.is_solid[x + (NY - 1) * NX] = 1;
	}
}

int main() {
	sf::ContextSettings settings;
	settings.antiAliasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode({900, 600}), "NACA simulation", sf::Style::Default, sf::State::Windowed, settings);
	sf::Vector2f viewSize({3 * 1.0f, 2 * 1.0f});
	window.setView(sf::View({0, 0}, viewSize));

	Foil foil(NACA(2412), 100);
	foil.setAngleOfAttack(5 * 3.1415 / 180);

	LBM lbm;
	buildSolid(foil, lbm, NX / viewSize.x);

	std::cout << "LBM started (NX=" << NX << " NY=" << NY << 
		" tau=" << tau << " nu=" << nu << " u_in=" << u_in << ")\n";

	auto t = time(NULL);
	int fps = 0;

	while (window.isOpen()) {
		while(auto e = window.pollEvent()) {
			if (e->is<sf::Event::Closed>())
				window.close();
			else if (auto w = e->getIf<sf::Event::MouseWheelScrolled>()) {
				foil.setAngleOfAttack(foil.getAngleOfAttack() + w->delta / 50);
				buildSolid(foil, lbm, NX / viewSize.x);
			}
		}

		sf::Texture txt(generateImg(lbm, false));
		txt.setSmooth(true);
		sf::Sprite sprite(txt);
		sprite.setScale({viewSize.x / NX, viewSize.y / NY});
		sprite.setPosition({-viewSize.x / 2, -viewSize.y / 2});
		
		window.clear(sf::Color(20, 20, 20));
		window.draw(sprite);

		window.draw(foil.lines);
		window.draw(foil.camberLine);
		window.draw(foil.upperFoil);
		window.draw(foil.lowerFoil);
		
		window.display();

		auto f = lbm.performSteps(10);

		if (time(NULL) == t)
			fps++;
		else {
			t = time(NULL);
			std::cout << "current fps: " << fps << ", Fx=" << f.first << ", Fy=" << f.second << ", L/D=" << f.second/f.first << "\n";
			fps = 0;
		}
	}

	return 0;
}
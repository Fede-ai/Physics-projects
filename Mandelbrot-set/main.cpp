#include <iostream>
#include "SFML/Graphics.hpp"

static constexpr size_t iter = 1'000;

size_t diverges(double r, double i);

int main() {
	unsigned int xSize = 15'000;
	double ratio = 0.882;
	double xView = 3;

	sf::Vector2u imgSize(xSize, unsigned int(xSize * ratio));
	sf::Image img(imgSize, sf::Color::Black);

	for (unsigned int x = 0; x < imgSize.x; x++) {
		for (unsigned int y = 0; y < imgSize.y / 2.f; y++) {
			double r = (int(x) - imgSize.x / 2.f) * xView / imgSize.x - 0.5;
			double i = (int(y) - imgSize.y / 2.f) * xView / imgSize.x;

			sf::Color c;
			auto n = diverges(r, i);
			auto s = std::pow(n / double(iter), 0.4);
			if (n == size_t(-1))
				c = sf::Color::Red;
			else if (n < iter)
				c = sf::Color(0, 255 * s, 255 * s);
			else
				c = sf::Color::White;

			img.setPixel({ x, y }, c);
			img.setPixel({ x, imgSize.y - y - 1 }, c);
		}

		if ((x + 1) % 100 == 0)
			std::cout << 100.f * (x + 1) / imgSize.x << "%\n";
	}

	auto _ = img.saveToFile("image.png");

	return 0; 
}

size_t diverges(double r, double i) {
	auto mod = [](double r, double i) {
		return sqrt(r * r + i * i);
		};

	if (mod(r, i) < 0.005)
		return size_t(-1);
	if (mod(r, i) > 2)
		return 0;

	double newR = r, newI = i;
	for (size_t a = 0; a < iter;) {
		a++;

		double tempR = newR * newR - newI * newI + r;
		double tempI = 2 * newR * newI + i;
		newR = tempR, newI = tempI;

		if (mod(newR, newI) > 2)
			return a;
	}

	return iter;
}
#include "pendulum.hpp"
#include <iostream>

Pendulum::Pendulum()
{
	const float width = sf::VideoMode::getFullscreenModes()[0].size.x * 2 / 3.f;
	defaultSize.y = width * 9 / 16.f;
	defaultSize.x = width - defaultSize.y * 0.8f;

	unit.x = int(std::round(defaultSize.x / 10.f));
	unit.y = int(std::round(defaultSize.y / 10.f));

	trail.push_back(pendulum);
}

void Pendulum::update(int fps)
{
	double dt = 1.0 / 1000.0 / fps;

	for (int i = 0; i < 1000; i++) {
		pendulum.y -= (pendulum.y * drag / 1000.f + 9.806 * sin(pendulum.x) / length) * dt;
		pendulum.x += pendulum.y * dt;
	}

	trail.push_back(pendulum);
}
sf::Image Pendulum::renderGraph(int width, int height)
{
	if (hasMoved) {
		lastImg = renderAxes(width, height);
		hasMoved = false;
	}
	sf::Image img = lastImg;

	//draw a pixel on the image
	auto draw = [this, &img](int x, int y, sf::Color c) {
		int xPos = int(x - anchor.x * zoom + img.getSize().x / 2.f);
		int yPos = int(y - anchor.y * zoom + img.getSize().y / 2.f);
		if (xPos >= 0 && xPos < int(img.getSize().x) &&
			yPos >= 0 && yPos < int(img.getSize().y))
			img.setPixel(sf::Vector2u(xPos, yPos), c);
		};

	//draw the trail
	if (trail.size() > 1) {
		for (int i = 0; i < trail.size() - 1; i++) {
			double x1 = trail[i].x * unit.x * zoom / pi * 2, x2 = trail[i + 1].x * unit.x * zoom / pi * 2;
			double y1 = -trail[i].y * unit.y * zoom / pi * 2, y2 = -trail[i + 1].y * unit.y * zoom / pi * 2;

			//dont render if both points are outside
			int x1p = int(x1 - anchor.x * zoom), y1p = int(y1 - anchor.y * zoom);
			int x2p = int(x2 - anchor.x * zoom), y2p = int(y2 - anchor.y * zoom);
			if (!(abs(x1p) < width / 2.f && abs(y1p) < height / 2.f) && 
				!(abs(x2p) < width / 2.f && abs(y2p) < height / 2.f))
				continue;

			if (abs(y2 - y1) > abs(x2 - x1)) {
				for (int y = 0; y < abs(y2 - y1); y++) {
					double a = y / abs(y2 - y1) * (y2 - y1);
					double b = a / (y2 - y1) * (x2 - x1);

					draw(int(std::round(x1 + b)), int(std::round(y1 + a)), sf::Color::White);
				}
			}
			else {
				for (int x = 0; x < abs(x2 - x1); x++) {
					double a = x / abs(x2 - x1) * (x2 - x1);
					double b = a / (x2 - x1) * (y2 - y1);

					draw(int(std::round(x1 + a)), int(std::round(y1 + b)), sf::Color::White);
				}
			}
		}
	}

	//draw the ball
	int radius = 8;
	int xPen = int(std::round(pendulum.x * unit.x * zoom / (pi / 2)));
	int yPen = -int(std::round(pendulum.y * unit.y * zoom / (pi / 2)));
	for (int x = -radius; x < radius + 1; x++) {
		for (int y = -radius; y < radius + 1; y++) {
			if (sqrt(x * x + y * y) < radius)
				draw(xPen + x, yPen + y, sf::Color(200, 10, 10));
		}
	}

	return img;
}

void Pendulum::moveGraph()
{
	sf::Vector2i mousePos = sf::Mouse::getPosition();

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && 
		isMovingGraph && mousePos != lastMousePos) {
		anchor.x += (lastMousePos.x - mousePos.x) / zoom;
		anchor.y += (lastMousePos.y - mousePos.y) / zoom;
		hasMoved = true;
	}

	lastMousePos = mousePos;
}
void Pendulum::applyZoom(float delta)
{
	zoom /= (1.f - delta / 10.f);
	zoom = std::max(zoom, 0.4f);
	
	hasMoved = true;
}

void Pendulum::setPendulum(int x, int y, int width, int height)
{
	int left = int(anchor.x * zoom - width / 2.f);
	int top = int(anchor.y * zoom - height / 2.f);

	pendulum.x = float(x + left) / (unit.x * zoom) * (pi / 2);
	pendulum.y = -float(y + top) / (unit.y * zoom) * (pi / 2);

	trail.clear();
	trail.push_back(pendulum);
}
void Pendulum::forceRerender()
{
	hasMoved = true;
}

sf::Image Pendulum::renderAxes(int width, int height) const
{
	sf::Image img(sf::Vector2u(width, height), sf::Color(50, 50, 50));
	int left = int(anchor.x * zoom - width / 2.f);
	int top = int(anchor.y * zoom - height / 2.f);

	//draw a pixel on the image
	auto draw = [this, &img](int x, int y, sf::Color c) {
		int xPos = int(x - anchor.x * zoom + img.getSize().x / 2.f);
		int yPos = int(y - anchor.y * zoom + img.getSize().y / 2.f);
		if (xPos >= 0 && xPos < int(img.getSize().x) && 
			yPos >= 0 && yPos < int(img.getSize().y))
			img.setPixel(sf::Vector2u(xPos, yPos), c);
		};

	//draw vertical axes
	for (int y = top; y < top + height; y++) {
		float xDiff = unit.x * zoom;
		for (float x = int(left / xDiff) * xDiff; x < left + width; x += xDiff)
			draw(int(std::round(x)), y, sf::Color(100, 100, 100));

		draw(0, y, sf::Color::Black);
		draw(1, y, sf::Color::Black);
	}

	//draw horizontal axes
	for (int x = left; x < left + width; x++) {
		if (x == 0 || x == 1)
			continue;

		float yDiff = unit.y * zoom;
		for (float y = int(top / yDiff) * yDiff; y < top + height; y += yDiff)
			draw(x, int(std::round(y)), sf::Color(100, 100, 100));

		draw(x, 0, sf::Color::Black);
		draw(x, 1, sf::Color::Black);
	}

	//draw all the vector arrows
	float yDiff = unit.y * zoom / 4.f;
	for (float y = int(top / yDiff - 1) * yDiff; y <= top + height + yDiff; y += yDiff) {

		float xDiff = unit.x * zoom / 8.f;
		for (float x = int(left / xDiff - 1) * xDiff; x <= left + width + xDiff; x += xDiff) {

			float xV = float(x / (unit.x * zoom) * pi / 2);
			float xSize = -y / (unit.y * zoom) * pi / 2;
			float ySize = float(xSize * drag / 1000.f + 9.806 * sin(xV) / length);

			xSize *= 0.9f, ySize *= 0.9f;
			float size = sqrt(xSize * xSize + ySize * ySize);
			float per = std::min(size / 15.f, 1.f);
			auto col = sf::Color(char(255 * (1 - per)), char(255 * per), 0);

			//draw the body of the arrow
			if (abs(xSize) > abs(ySize)) {
				for (int i = 0; i < abs(xSize * zoom); i++) {
					float a = i / abs(xSize) * xSize;
					float b = a / xSize * ySize;
					draw(int(std::round(x + a)), int(std::round(y + b)), col);
				}
			}
			else {
				for (int i = 0; i < abs(ySize * zoom); i++) {
					float a = i / abs(ySize) * ySize;
					float b = a / ySize * xSize;
					draw(int(std::round(x + b)), int(std::round(y + a)), col);
				}
			}

			//draw the base of the tip
			float m = -xSize / ySize;
			float dx = cos(atan(m)) * zoom * size / 5.f;
			float dy = sin(atan(m)) * zoom * size / 5.f;
			if (abs(dy) > abs(dx)) {
				for (int i = 0; i < abs(dy); i++) {
					double a = i / abs(dy) * (dy);
					double b = a / (dy) * (dx);

					draw(int(std::round(x + xSize * zoom + b)), int(std::round(y + ySize * zoom + a)), col);
					draw(int(std::round(x + xSize * zoom - b)), int(std::round(y + ySize * zoom - a)), col);
				}
			}
			else {
				for (int i = 0; i < abs(dx); i++) {
					double a = i / abs(dx) * (dx);
					double b = a / (dx) * (dy);

					draw(int(std::round(x + xSize * zoom + a)), int(std::round(y + ySize * zoom + b)), col);
					draw(int(std::round(x + xSize * zoom - a)), int(std::round(y + ySize * zoom - b)), col);
				}
			}

			//draw right side of the tip
			float tx = 1.2f * xSize * zoom, ty = 1.2f * ySize * zoom;
			float d2x = xSize * zoom + dx - tx;
			float d2y = ySize * zoom + dy - ty;
			if (abs(d2y) > abs(d2x)) {
				for (int i = 0; i < abs(d2y); i++) {
					double a = i / abs(d2y) * (d2y);
					double b = a / (d2y) * (d2x);

					draw(int(std::round(x + tx + b)), int(std::round(y + ty + a)), col);
				}
			}
			else {
				for (int i = 0; i < abs(d2x); i++) {
					double a = i / abs(d2x) * (d2x);
					double b = a / (d2x) * (d2y);

					draw(int(std::round(x + tx + a)), int(std::round(y + ty + b)), col);
				}
			}

			//draw left side of the tip
			d2x = xSize * zoom - dx - tx;
			d2y = ySize * zoom - dy - ty;
			if (abs(d2y) > abs(d2x)) {
				for (int i = 0; i < abs(d2y); i++) {
					double a = i / abs(d2y) * (d2y);
					double b = a / (d2y) * (d2x);

					draw(int(std::round(x + tx + b)), int(std::round(y + ty + a)), col);
				}
			}
			else {
				for (int i = 0; i < abs(d2x); i++) {
					double a = i / abs(d2x) * (d2x);
					double b = a / (d2x) * (d2y);

					draw(int(std::round(x + tx + a)), int(std::round(y + ty + b)), col);
				}
			}
		}
	}

	return img;
}

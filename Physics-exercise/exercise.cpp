#include "exercise.h"
#include <iostream>
constexpr auto PI = 3.14159265358979323846;

Exercise::Exercise()
{	
	hydro.setPrimitiveType(sf::TriangleStrip);
	hydro.append(sf::Vertex(sf::Vector2f(6404, 640)));
	hydro.append(sf::Vertex(sf::Vector2f(6500, 640)));
	hydro.append(sf::Vertex(sf::Vector2f(6404, 450)));
	hydro.append(sf::Vertex(sf::Vector2f(6500, 540)));
	hydro.append(sf::Vertex(sf::Vector2f(7000 - 180, 450)));
	hydro.append(sf::Vertex(sf::Vector2f(6500 + 180, 540)));
	hydro.append(sf::Vertex(sf::Vector2f(7000 - 180, 1156)));
	hydro.append(sf::Vertex(sf::Vector2f(6500 + 180, 1420)));
	hydro.append(sf::Vertex(sf::Vector2f(7000, 1156)));
	hydro.append(sf::Vertex(sf::Vector2f(7000, 1420)));
	for (int i = 0; i < 10; i++)
		hydro[i].color = sf::Color(40, 70, 170);

	font.loadFromFile("font.ttf");
	staticTxt.setFont(font);

	water.setPosition(7100 + cos(35.f / 180 * PI) * 13 * meter + 800, 1156 - sin(35.f / 180 * PI) * 13 * meter - 9 * meter);
	water.setFillColor(sf::Color(40, 70, 170));

	springTexture.loadFromFile("spring.png");
	springTexture.setSmooth(true);
	spring.setTexture(&springTexture);
	spring.setOrigin(0, 0.1 * meter);
	spring.setRotation(atan(2 / 3.f) * 180 / PI);
	spring.setPosition(782.5, 458);

	sphere1.setFillColor(sf::Color(100, 210, 255));
	sphere1.setRadius(0.14 * meter);
	sphere1.setOrigin(0.14 * meter, 0.14 * meter); 
	sphere1.setOutlineThickness(-6);
	sphere1.setOutlineColor(sf::Color::Black);
	line1.setSize(sf::Vector2f(10, 0.14 * meter));
	line1.setOrigin(5, 0);
	line1.setFillColor(sf::Color::Black);

	sphere2.setFillColor(sf::Color(100, 210, 255));
	sphere2.setRadius(0.1 * meter);
	sphere2.setOrigin(0.1 * meter, 0.1 * meter);
	sphere2.setOutlineThickness(-6);
	sphere2.setOutlineColor(sf::Color::Black);
	line2.setSize(sf::Vector2f(10, 0.1 * meter));
	line2.setOrigin(5, 0);
	line2.setFillColor(sf::Color::Black);

	sphere3.setFillColor(sf::Color(100, 210, 255));
	sphere3.setRadius(0.155 * meter);
	sphere3.setOrigin(0.155 * meter, 0.155 * meter);
	sphere3.setOutlineThickness(-6);
	sphere3.setOutlineColor(sf::Color::Black);
	line3.setSize(sf::Vector2f(10, 0.155 * meter));
	line3.setOrigin(5, 0);
	line3.setFillColor(sf::Color::Black);

	sphere4.setFillColor(sf::Color(100, 210, 255));
	sphere4.setRadius(0.1 * meter);
	sphere4.setOrigin(0.1 * meter, 0.1 * meter);
	sphere4.setOutlineThickness(-6);
	sphere4.setOutlineColor(sf::Color::Black);

	sphere5.setFillColor(sf::Color(100, 210, 255));
	sphere5.setRadius(0.14 * meter);
	sphere5.setOrigin(0.14 * meter, 0.14 * meter);
	sphere5.setOutlineThickness(-6);
	sphere5.setOutlineColor(sf::Color::Black);

	pedana.setFillColor(sf::Color(100, 100, 100));
	pedana.setSize(sf::Vector2f(0.32 * meter , 60));
	pedana.setOrigin(0.32 * meter, 60);
	pedana.setOutlineThickness(-6);
	pedana.setOutlineColor(sf::Color::Black);

	pistone.setFillColor(sf::Color(100, 100, 100));
	pistone.setSize(sf::Vector2f(40, 0.88 * meter));
	pistone.setOrigin(40, 0.88 * meter);
	pistone.setOutlineThickness(-6);
	pistone.setOutlineColor(sf::Color::Black);
	
	cube.setFillColor(sf::Color(150, 110, 80));
	cube.setSize(sf::Vector2f(0.4 * meter, 0.4 * meter));
	cube.setOrigin(0.2 * meter, 0.2 * meter);
	cube.setOutlineThickness(-6);
	cube.setOutlineColor(sf::Color::Black);

	sphere6.setFillColor(sf::Color(150, 110, 80));
	sphere6.setRadius(0.26 * meter);
	sphere6.setOrigin(0.26 * meter, 0.26 * meter);
	sphere6.setOutlineThickness(-6);
	sphere6.setOutlineColor(sf::Color::Black);
}

int Exercise::run()
{
	sf::Vector2f windowSize;
	windowSize.x = sf::VideoMode::getDesktopMode().width * 2 / 3;
	windowSize.y = windowSize.x * 9 / 16;

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	window.create(sf::VideoMode(windowSize.x, windowSize.y), "Physics exercise", sf::Style::Default, settings);
	window.setFramerateLimit(100);	
	window.setView(sf::View(sf::Vector2f(1920 / 2, 1080 / 2), sf::Vector2f(1920, -1080)));
	lastTime = time();

	moveGround();
	updateAndDrawObj();

	while (window.isOpen())
	{
		sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition());
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (event.type == sf::Event::MouseWheelScrolled)
			{
				sf::View view(window.getView());
				view.zoom(1 - 0.1 * event.mouseWheelScroll.delta);
				window.setView(view);
				moveGround();
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Enter && event.key.alt && canFullscreen)
				{
					sf::View view = window.getView();
					if (!isFullscreen)
						window.create(sf::VideoMode::getDesktopMode(), "Physics exercise", sf::Style::Fullscreen, settings);
					else
						window.create(sf::VideoMode(windowSize.x, windowSize.y), "Physics exercise", sf::Style::Default, settings);
					window.setView(view);
					window.setFramerateLimit(60);
					canFullscreen = false;
					isFullscreen = !isFullscreen;
				}
				else if (event.key.code == sf::Keyboard::Space)
					isPaused = !isPaused;
				else if (event.key.code == sf::Keyboard::R || event.key.code == sf::Keyboard::Num0)
					exerciseTime = 0;
				else if (event.key.code == sf::Keyboard::Left)
					exerciseTime = std::max(exerciseTime - 800, 0);
				else if (event.key.code == sf::Keyboard::Right)
					exerciseTime = std::min(exerciseTime + 800, 15000);
				else if (event.key.code == sf::Keyboard::Up)
					speed = std::min(speed + 0.1, double(2));
				else if (event.key.code == sf::Keyboard::Down)
					speed = std::max(speed - 0.1, double(-2));
				else if (event.key.code == sf::Keyboard::Num1)
					exerciseTime = 480;
				else if (event.key.code == sf::Keyboard::Num2)
					exerciseTime = (0.47 + 2.908) * 1000;
				else if (event.key.code == sf::Keyboard::Num3)
					exerciseTime = (0.4674 + 2.92 + 0.8 + 1.35 + 2) * 1000;
				else if (event.key.code == sf::Keyboard::Num4)
					exerciseTime = (0.4674 + 2.91 + 0.8 + 1.35 + 2 + 3.04 + 0.28) * 1000;
				else if (event.key.code == sf::Keyboard::Num5)
					exerciseTime = 15000;
				updateAndDrawObj();
			}
		}

		window.clear();

		background.setPrimitiveType(sf::TriangleFan);
		background.clear();
		background.resize(22);
		background[0].position = window.getView().getCenter();
		background[0].color = sf::Color(0, 200, 200);
		int radius = 775.f * window.getView().getSize().x / 1920.f;
		for (int i = 0; i < 21; i++)
		{
			float ang = 2 * PI * (float(i) / (20));
			sf::Vector2f pos(cos(ang) * radius * 16 / 9, sin(ang) * radius);
			background[i + 1].position = pos + window.getView().getCenter();
			background[i + 1].color = sf::Color(0, 50, 130);
		}
		window.draw(background);
		window.draw(ground);
		window.draw(hydro);
		window.draw(water);

		updateAndDrawObj();

		window.display();		
		
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
			canFullscreen = true;		

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus())
		{
			sf::View view(window.getView());
			float x = window.getView().getSize().x / window.getSize().x;
			float y = window.getView().getSize().y / window.getSize().y;
			view.move((lastMousePos - mousePos).x * x, (lastMousePos - mousePos).y * y);
			window.setView(view);
			moveGround();
		}
		
		if (!isPaused)
			exerciseTime += (time() - lastTime) * speed;
		lastTime = time();
		lastMousePos = mousePos;
	}

	return 0;
}

void Exercise::updateAndDrawObj()
{
	exerciseTime = std::min(std::max(exerciseTime, 0), 15000);
	double seconds = exerciseTime / 1000.f;
	double secSinceLast;

	sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	std::cout << mousePos.x << ", " << mousePos.y << "\n";

	if (seconds <= 0.4674)
	{
		spring.setSize(sf::Vector2f(std::min((0.25 + 3.4 * seconds) * meter, 0.625 * meter), 0.24 * meter));
		window.draw(spring);

		double space = 3.6 * seconds * meter - 1 / 2.f * 3.851 * seconds * seconds * meter;
		sphere1.setPosition(874 + cos(atan(2/3.f)) * space, 700 + 0.14 * meter - 0.7 * meter + sin(atan(2 / 3.f)) * space);
		window.draw(sphere1);
		line1.setPosition(sphere1.getPosition());
		line1.setRotation(214-space / sphere1.getRadius() * 180 / PI);
		window.draw(line1);

		sphere2.setPosition(1189.01 + sphere1.getRadius() + sphere2.getRadius() + 3.97 * meter + 3 * meter * (seconds - 0.4574), 700 + 0.10 * meter);
		window.draw(sphere2);
		line2.setPosition(sphere2.getPosition());
		line2.setRotation(-21 - 3 * meter * (seconds - 0.4574) / sphere1.getRadius() * 180 / PI);
		window.draw(line2);

		pedana.setPosition(6500, 700);
		window.draw(pedana);
		pistone.setPosition(7000, 1420);
		window.draw(pistone);

		cube.setRotation(0);
		cube.setPosition(7000 + 0.2 * meter, 1420 - 0.88 * meter + 0.2 * meter);
		window.draw(cube);

		sphere6.setPosition(7800 + cos(35.f / 180 * PI) * 13 * meter + 0.26 * meter, 1156 - sin(35.f / 180 * PI) * 13 * meter + 0.26 * meter);
		window.draw(sphere6);
	}
	else if (seconds <= 0.4674 + 2.91)
	{
		spring.setSize(sf::Vector2f(0.625 * meter, 0.24 * meter));
		window.draw(spring);

		secSinceLast = seconds - 0.4674;
		sphere1.setPosition(1189.01 + 1.8 * meter * secSinceLast + 1.78 / 2 * secSinceLast * secSinceLast * meter, 700 + 0.14 * meter);
		window.draw(sphere1);
		double space = 3.6 * 0.4674 * meter - 1 / 2.f * 3.851 * 0.4674 * 0.4674 * meter;
		line1.setPosition(sphere1.getPosition());
		line1.setRotation(214 - (space + +1.8 * meter * secSinceLast + 1.78 / 2 * secSinceLast * secSinceLast * meter) / sphere1.getRadius() * 180 / PI);
		window.draw(line1);

		sphere2.setPosition(1189.01 + sphere1.getRadius() + sphere2.getRadius() + 4 * meter + 3 * meter * secSinceLast, 700 + 0.10 * meter);
		window.draw(sphere2);
		line2.setPosition(sphere2.getPosition());
		line2.setRotation(-21 - 3 * meter * secSinceLast / sphere2.getRadius() * 180 / PI);
		window.draw(line2);

		pedana.setPosition(6500, 700);
		window.draw(pedana);
		pistone.setPosition(7000, 1420);
		window.draw(pistone);

		cube.setRotation(0);
		cube.setPosition(7000 + 0.2 * meter, 1420 - 0.88 * meter + 0.2 * meter);
		window.draw(cube);

		sphere6.setPosition(7800 + cos(35.f / 180 * PI) * 13 * meter + 0.26 * meter, 1156 - sin(35.f / 180 * PI) * 13 * meter + 0.26 * meter);
		window.draw(sphere6);
	}
	else if (seconds <= 0.4674 + 2.91 + 0.8)
	{
		secSinceLast = seconds - 0.4674 - 2.91;
		spring.setSize(sf::Vector2f(0.625 * meter, 0.24 * meter));
		window.draw(spring);

		pedana.setPosition(6500, 700);
		window.draw(pedana);
		pistone.setPosition(7000, 1420);
		window.draw(pistone);

		sphere3.setPosition(5050 + 5.9 * meter * secSinceLast, 700 + 0.155 * meter);
		window.draw(sphere3);
		line3.setPosition(sphere3.getPosition());
		line3.setRotation(180-5.9 * meter * secSinceLast / sphere3.getRadius() * 180 / PI);
		window.draw(line3);

		cube.setRotation(0);
		cube.setPosition(7000 + 0.2 * meter, 1420 - 0.88 * meter + 0.2 * meter);
		window.draw(cube);

		sphere6.setPosition(7800 + cos(35.f / 180 * PI) * 13 * meter + 0.26 * meter, 1156 - sin(35.f / 180 * PI) * 13 * meter + 0.26 * meter);
		window.draw(sphere6);
	}
	else if (seconds <= 0.4674 + 2.91 + 0.8 + 1.35)
	{
		secSinceLast = seconds - 0.4674 - 2.91 - 0.8;

		spring.setSize(sf::Vector2f(0.625 * meter, 0.24 * meter));
		window.draw(spring);		
		
		pedana.setPosition(6500, 700);
		window.draw(pedana);
		pistone.setPosition(7000, 1420);
		window.draw(pistone);

		sphere5.setPosition(6500 - 0.14 * meter, 700 + 0.14 * meter);
		window.draw(sphere5);

		sphere4.setPosition(6500 - 0.14 * meter + secSinceLast * meter * (secSinceLast - 2.7), 700 + 0.10 * meter);
		window.draw(sphere4);
		line2.setPosition(sphere4.getPosition());
		line2.setRotation(180 - (secSinceLast * meter * (secSinceLast - 2.7)) / sphere4.getRadius() * 180 / PI);
		window.draw(line2);

		cube.setRotation(0);
		cube.setPosition(7000 + 0.2 * meter, 1420 - 0.88 * meter + 0.2 * meter);
		window.draw(cube);

		sphere6.setPosition(7800 + cos(35.f / 180 * PI) * 13 * meter + 0.26 * meter, 1156 - sin(35.f / 180 * PI) * 13 * meter + 0.26 * meter);
		window.draw(sphere6);
	}
	else if (seconds <= 0.4674 + 2.91 + 0.8 + 1.35 + 2)
	{
		secSinceLast = seconds - 0.4674 - 2.91 - 0.8 - 1.35;

		spring.setSize(sf::Vector2f(0.625 * meter, 0.24 * meter));
		window.draw(spring);

		pedana.setPosition(6500, 700 - 0.1 * secSinceLast * meter);
		window.draw(pedana);
		pistone.setPosition(7000 + 2 / 30.f * secSinceLast * meter, 1420);
		window.draw(pistone);

		sphere4.setPosition(6500 - 0.14 * meter + 1.35 * meter * (1.35 - 2.7), 700 + 0.10 * meter);
		window.draw(sphere4);

		sphere5.setPosition(6500 - 0.14 * meter, 700 + 0.14 * meter - 0.1 * secSinceLast * meter);
		window.draw(sphere5);

		cube.setRotation(0);
		cube.setPosition(7000 + 2 / 30.f * secSinceLast * meter + 0.2 * meter, 1420 - 0.88 * meter + 0.2 * meter);
		window.draw(cube);

		sphere6.setPosition(7800 + cos(35.f / 180 * PI) * 13 * meter + 0.26 * meter, 1156 - sin(35.f / 180 * PI) * 13 * meter + 0.26 * meter);
		window.draw(sphere6);
	}
	else if (seconds <= 0.4674 + 2.91 + 0.8 + 1.35 + 2 + 3.04)
	{
		secSinceLast = seconds - 0.4674 - 2.91 - 0.8 - 1.35 - 2;
		spring.setSize(sf::Vector2f(0.625 * meter, 0.24 * meter));
		window.draw(spring);

		pedana.setPosition(6500, 700 - 0.1 * 2 * meter);
		window.draw(pedana);
		pistone.setPosition(7000 + 2 / 30.f * 2 * meter, 1420);
		window.draw(pistone);

		sphere4.setPosition(6500 - 0.14 * meter + 1.35 * meter * (1.35 - 2.7), 700 + 0.10 * meter);
		window.draw(sphere4);

		sphere5.setPosition(6500 - 0.14 * meter, 700 + 0.14 * meter - 0.1 * 2 * meter);
		window.draw(sphere5);

		double dist = 1 / 2.f * secSinceLast * secSinceLast * 2.81 * meter;
		cube.setRotation(55);
		cube.setPosition(7100 + cos(55 / 180.f * PI) * 0.2 * meter + sin(55 / 180.f * PI) * dist, 
			1420 - 0.88 * meter + sin(55 / 180.f * PI) * 0.2 * meter - cos(55 / 180.f * PI) * dist);
		window.draw(cube);

		sphere6.setPosition(7800 + cos(35.f / 180 * PI) * 13 * meter + 0.26 * meter, 1156 - sin(35.f / 180 * PI) * 13 * meter + 0.26 * meter);
		window.draw(sphere6);
	}
	else if (seconds <= 0.4674 + 2.91 + 0.8 + 1.35 + 2 + 3.04 + 0.28)
	{
		secSinceLast = seconds - 0.4674 - 2.91 - 0.8 - 1.35 - 2 - 3.04;
		spring.setSize(sf::Vector2f(0.625 * meter, 0.24 * meter));
		window.draw(spring);

		pedana.setPosition(6500, 700 - 0.1 * 2 * meter);
		window.draw(pedana);
		pistone.setPosition(7000 + 2 / 30.f * 2 * meter, 1420);
		window.draw(pistone);

		sphere4.setPosition(6500 - 0.14 * meter + 1.35 * meter * (1.35 - 2.7), 700 + 0.10 * meter);
		window.draw(sphere4);

		sphere5.setPosition(6500 - 0.14 * meter, 700 + 0.14 * meter - 0.1 * 2 * meter);
		window.draw(sphere5);

		cube.setRotation(0);
		cube.setPosition(7100 + cos(35.f / 180 * PI) * 13 * meter + 0.2 * meter + 7 * secSinceLast * meter,
			1156 - sin(35.f / 180 * PI) * 13 * meter + 0.2 * meter);
		window.draw(cube);

		sphere6.setPosition(7800 + cos(35.f / 180 * PI) * 13 * meter + 0.26 * meter, 1156 - sin(35.f / 180 * PI) * 13 * meter + 0.26 * meter);
		window.draw(sphere6);
	}
	else if (seconds <= 0.4674 + 2.91 + 0.8 + 1.35 + 2 + 3.04 + 0.28 + 1.3546)
	{
		secSinceLast = seconds - 0.4674 - 2.91 - 0.8 - 1.35 - 2 - 3.04 - 0.28;
		spring.setSize(sf::Vector2f(0.625 * meter, 0.24 * meter));
		window.draw(spring);

		pedana.setPosition(6500, 700 - 0.1 * 2 * meter);
		window.draw(pedana);
		pistone.setPosition(7000 + 2 / 30.f * 2 * meter, 1420);
		window.draw(pistone);

		sphere4.setPosition(6500 - 0.14 * meter + 1.35 * meter * (1.35 - 2.7), 700 + 0.10 * meter);
		window.draw(sphere4);

		sphere5.setPosition(6500 - 0.14 * meter, 700 + 0.14 * meter - 0.1 * 2 * meter);
		window.draw(sphere5);

		cube.setRotation(0);
		cube.setPosition(std::max(7100 + cos(35.f / 180 * PI) * 13 * meter + 0.2 * meter + 7 * 0.28 * meter - 0.49 * secSinceLast * meter, 
			7100 + cos(35.f / 180 * PI) * 13 * meter + 0.2 * meter),
			1156 - sin(35.f / 180 * PI) * 13 * meter + 0.2 * meter);
		window.draw(cube);

		sphere6.setPosition(7800 + cos(35.f / 180 * PI) * 13 * meter + 0.26 * meter + 6.51 * meter * secSinceLast, 
			1156 - sin(35.f / 180 * PI) * 13 * meter + 0.26 * meter - 9.81 / 2 * meter * secSinceLast * secSinceLast);
		window.draw(sphere6);
	}
	else if (seconds <= 0.4674 + 2.91 + 0.8 + 1.35 + 2 + 3.04 + 0.28 + 1.3546 + 1.081)
	{
		secSinceLast = seconds - 0.4674 - 2.91 - 0.8 - 1.35 - 2 - 3.04 - 0.28 - 1.3546;
		spring.setSize(sf::Vector2f(0.625 * meter, 0.24 * meter));
		window.draw(spring);

		pedana.setPosition(6500, 700 - 0.1 * 2 * meter);
		window.draw(pedana);
		pistone.setPosition(7000 + 2 / 30.f * 2 * meter, 1420);
		window.draw(pistone);

		sphere4.setPosition(6500 - 0.14 * meter + 1.35 * meter * (1.35 - 2.7), 700 + 0.10 * meter);
		window.draw(sphere4);

		sphere5.setPosition(6500 - 0.14 * meter, 700 + 0.14 * meter - 0.1 * 2 * meter);
		window.draw(sphere5);

		cube.setRotation(0);
		cube.setPosition(std::max(7100 + cos(35.f / 180 * PI) * 13 * meter + 0.2 * meter + 7 * 0.28 * meter - 0.49 * (secSinceLast + 1.3546) * meter,
			7100 + cos(35.f / 180 * PI) * 13 * meter + 0.2 * meter),
			1156 - sin(35.f / 180 * PI) * 13 * meter + 0.2 * meter);
		window.draw(cube);

		sphere6.setPosition(7800 + cos(35.f / 180 * PI) * 13 * meter + 0.26 * meter + 6.51 * meter * 1.3546 + 6.51 * meter * secSinceLast
			- 4.f / 2 * meter * secSinceLast * secSinceLast,
			1156 - sin(35.f / 180 * PI) * 13 * meter + 0.26 * meter - 9.81 / 2 * meter * (1.3546) * (1.3546)
			+ 12 * meter * secSinceLast * secSinceLast - 9.81 * 1.3546 * secSinceLast * meter);
		window.draw(sphere6);
	}
	else
	{
		secSinceLast = seconds - 13.283;
		spring.setSize(sf::Vector2f(0.625 * meter, 0.24 * meter));
		window.draw(spring);

		pedana.setPosition(6500, 700 - 0.1 * 2 * meter);
		window.draw(pedana);
		pistone.setPosition(7000 + 2 / 30.f * 2 * meter, 1420);
		window.draw(pistone);

		sphere4.setPosition(6500 - 0.14 * meter + 1.35 * meter * (1.35 - 2.7), 700 + 0.10 * meter);
		window.draw(sphere4);

		sphere5.setPosition(6500 - 0.14 * meter, 700 + 0.14 * meter - 0.1 * 2 * meter);
		window.draw(sphere5);

		cube.setRotation(0);
		cube.setPosition(std::max(7100 + cos(35.f / 180 * PI) * 13 * meter + 0.2 * meter + 7 * 0.28 * meter - 0.49 * (1.1 + secSinceLast + 1.3546) * meter,
			7100 + cos(35.f / 180 * PI) * 13 * meter + 0.2 * meter),
			1156 - sin(35.f / 180 * PI) * 13 * meter + 0.2 * meter);
		window.draw(cube);

		sphere6.setPosition(7800 + cos(35.f / 180 * PI) * 13 * meter + 0.26 * meter + 6.51 * meter * 1.3546 + 6.51 * meter * 1.08
			- 4.f / 2 * meter * 1.08 * 1.08,
			1156 - sin(35.f / 180 * PI) * 13 * meter + 0.26 * meter - 9.81 / 2 * meter * (1.3546) * (1.3546)
			+ 12 * meter * 1.08 * 1.08 - 9.81 * 1.3546 * 1.08 * meter);
		window.draw(sphere6);
	}

	sf::RectangleShape measure;
	measure.setSize(sf::Vector2f(4 * meter, 10));
	measure.setPosition(1200 + sphere1.getRadius(), 800);
	measure.setFillColor(sf::Color::Black);
	window.draw(measure);
	sf::Text txt("4 m", font, 150);
	txt.setScale(1, -1);
	txt.setPosition(1760, 980);
	txt.setFillColor(sf::Color::Black);
	window.draw(txt);

	measure.setSize(sf::Vector2f(10, -0.7 * meter));
	measure.setPosition(1231.04, 700.078);
	window.draw(measure);
	txt.setCharacterSize(80);
	txt.setString("0.7 m");
	txt.setPosition(1266.8, 637.134);
	window.draw(txt);

	measure.setSize(sf::Vector2f(0.25 * meter, 10));
	measure.setPosition(743.287, 516.99);
	measure.setRotation(33.69);
	window.draw(measure);
	txt.setString("0.25 m");
	txt.setPosition(622.5, 559.5);
	txt.setRotation(33.69);
	window.draw(txt);

	measure.setSize(sf::Vector2f(13 * meter, 10));
	measure.setPosition(7065, 1107);
	measure.setRotation(-35);
	window.draw(measure);
	txt.setCharacterSize(150);
	txt.setString("13 m");
	txt.setPosition(8420.97, 123.803);
	txt.setRotation(-35);
	window.draw(txt);

	measure.setSize(sf::Vector2f(10, 9 * meter));
	measure.setPosition(11030, -1080);
	measure.setRotation(180);
	window.draw(measure);
	txt.setString("9 m");
	txt.setPosition(10730, -2395.37);
	txt.setRotation(0);
	window.draw(txt);

	//txt.setCharacterSize(40);
	//txt.setString("r = 0.14 cm\n2600 kg/m^3");
	//txt.setPosition(sphere1.getPosition() + sf::Vector2f(-txt.getGlobalBounds().width/2, 100));
	//window.draw(txt);

	double mult = window.getView().getSize().x / 1920.f;
	staticTxt.setCharacterSize(50);
	staticTxt.setScale(mult, -mult);
	staticTxt.setPosition(window.getView().getCenter() + sf::Vector2f(-940 * mult, 530 * mult));
	std::string num = std::to_string(speed);
	while (num[num.size() - 1] == '0' && num.size() > 4)
		num.erase(num.end() - 1);
	std::string tempo = std::to_string(exerciseTime / 1000.f);
	while (tempo[tempo.size() - 1] == '0' && tempo.size() > 6)
		tempo.erase(tempo.end() - 1);
	staticTxt.setString("Velocità: " + num);
	window.draw(staticTxt);
	staticTxt.setPosition(window.getView().getCenter() + sf::Vector2f(590 * mult, 530 * mult));
	staticTxt.setString(+"Tempo: " + tempo);
	window.draw(staticTxt);
}

void Exercise::moveGround()
{
	double bot = window.getView().getCenter().y + window.getView().getSize().y / 2.f;
	double left = window.getView().getCenter().x - window.getView().getSize().x / 2.f;
	double right = window.getView().getCenter().x + window.getView().getSize().x / 2.f;
	ground.clear();
	ground.setPrimitiveType(sf::TriangleStrip);
	ground.append(sf::Vertex(sf::Vector2f(left, 300)));
	ground.append(sf::Vertex(sf::Vector2f(left, bot)));
	ground.append(sf::Vertex(sf::Vector2f(600, 300)));
	ground.append(sf::Vertex(sf::Vector2f(1200, bot)));
	ground.append(sf::Vertex(sf::Vector2f(1200, 700)));
	ground.append(sf::Vertex(sf::Vector2f(6404, bot)));
	ground.append(sf::Vertex(sf::Vector2f(6404, 700)));
	ground.append(sf::Vertex(sf::Vector2f(6404, bot)));
	ground.append(sf::Vertex(sf::Vector2f(6404, 640)));
	ground.append(sf::Vertex(sf::Vector2f(6500, 640)));
	ground.append(sf::Vertex(sf::Vector2f(6404, bot)));
	ground.append(sf::Vertex(sf::Vector2f(6500, bot)));
	ground.append(sf::Vertex(sf::Vector2f(7000, bot)));
	ground.append(sf::Vertex(sf::Vector2f(6500, 1500)));
	ground.append(sf::Vertex(sf::Vector2f(7000, 1500)));
	ground.append(sf::Vertex(sf::Vector2f(7000, bot)));
	ground.append(sf::Vertex(sf::Vector2f(7000, 1156)));
	ground.append(sf::Vertex(sf::Vector2f(7100, 1156)));
	ground.append(sf::Vertex(sf::Vector2f(7000, bot)));
	ground.append(sf::Vertex(sf::Vector2f(7100 + cos(35.f / 180 * PI) * 13 * meter, 1156 - sin(35.f / 180 * PI) * 13 * meter)));
	ground.append(sf::Vertex(sf::Vector2f(7100 + cos(35.f / 180 * PI) * 13 * meter, bot)));
	ground.append(sf::Vertex(sf::Vector2f(7100 + cos(35.f / 180 * PI) * 13 * meter + 800, 1156 - sin(35.f / 180 * PI) * 13 * meter)));
	ground.append(sf::Vertex(sf::Vector2f(7100 + cos(35.f / 180 * PI) * 13 * meter + 800, bot)));

	water.setSize(sf::Vector2f(right - water.getPosition().x, water.getPosition().y - bot));
	water.setOrigin(0, water.getSize().y);

	for (int i = 0; i < 23; i++)
		ground[i].color = sf::Color(160, 160, 160);
}

size_t Exercise::time()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
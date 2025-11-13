#include "pendulum.h"
#include <iostream>
constexpr auto PI = 3.14159265358979323846;

Pendulum::Pendulum()
{
	windowSize.x = sf::VideoMode::getDesktopMode().width * 2 / 3;
	windowSize.y = windowSize.x * 9 / 16;

	settings.antialiasingLevel = 4;

	int n = 21;
	background.setPrimitiveType(sf::TriangleFan);
	background.resize(n + 1);
	background[0].position = sf::Vector2f(0, 0);
	background[0].color = sf::Color(0, 200, 200);
	int radius = 775;
	for (int i = 0; i < n; i++)
	{
		float ang = 2 * PI * (float(i) / (n-1));
		sf::Vector2f pos(cos(ang)*radius * 16 / 9, sin(ang)*radius);
		background[i + 1].position = pos;
		background[i + 1].color = sf::Color(0, 50, 130);
	}

	firstStart.setRadius(firstSize.y / 2);
	firstStart.setOrigin(firstSize.y / 2, firstSize.y / 2);
	firstBody.setSize(sf::Vector2f(firstSize));
	firstBody.setOrigin(0, firstSize.y / 2);
	firstEnd = firstStart;

	secondStart.setRadius(secondSize.y / 2);
	secondStart.setOrigin(secondSize.y / 2, secondSize.y / 2);
	secondStart.setFillColor(sf::Color::Black);
	secondBody.setSize(sf::Vector2f(secondSize));
	secondBody.setOrigin(0, secondSize.y / 2);
	secondBody.setFillColor(sf::Color::Black);
	secondEnd = secondStart;

	thirdStart.setRadius(thirdSize.y / 2);
	thirdStart.setOrigin(thirdSize.y / 2, thirdSize.y / 2);
	thirdStart.setFillColor(sf::Color::Red);
	thirdBody.setSize(sf::Vector2f(thirdSize));
	thirdBody.setOrigin(0, thirdSize.y / 2);
	thirdBody.setFillColor(sf::Color::Red);
	thirdEnd = thirdStart;

	font.loadFromFile("font.ttf");
	text.setFont(font);
	text.setCharacterSize(60);
	clock.restart();
}

int Pendulum::run()
{
	bool isFullscreen = false;
	window.create(sf::VideoMode(windowSize.x, windowSize.y), "Pendulum", sf::Style::Default, settings);
	window.setView(sf::View(sf::Vector2f(0, 0), sf::Vector2f(1920, 1080)));
	window.setFramerateLimit(60);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				double thirdBefore = thirdSpeed;
				double secondBefore = secondSpeed;

				if (event.key.code == sf::Keyboard::R)
				{
					firstAng = 0;
					secondAng = 0;
					thirdAng = 0;
					secondTray.clear();
					thirdTray.clear();
				}
				else if (event.key.code == sf::Keyboard::Enter)
				{
					if (event.key.alt)
					{
						if (!isFullscreen)
							window.create(sf::VideoMode::getDesktopMode(), "Pendulum", sf::Style::Fullscreen, settings);
						else
							window.create(sf::VideoMode(windowSize.x, windowSize.y), "Pendulum", sf::Style::Default, settings);
						window.setView(sf::View(sf::Vector2f(0, 0), sf::Vector2f(1920, 1080)));
						window.setFramerateLimit(60);
						isFullscreen = !isFullscreen;
					}
				}
				else if (event.key.code == sf::Keyboard::X)
					drawSecond = !drawSecond;
				else if (event.key.code == sf::Keyboard::C)
					drawThird = !drawThird;
				else if (event.key.code == sf::Keyboard::Q)
					firstSpeed = std::min(double(3), firstSpeed + 0.1);
				else if (event.key.code == sf::Keyboard::A)
					firstSpeed = std::max(double(-3), firstSpeed - 0.1);
				else if (event.key.code == sf::Keyboard::W)
					secondSpeed = std::min(double(5), secondSpeed + 0.1);
				else if (event.key.code == sf::Keyboard::S)
					secondSpeed = std::max(double(-5), secondSpeed - 0.1);
				else if (event.key.code == sf::Keyboard::E)
					thirdSpeed = std::min(double(5), thirdSpeed + 0.1);
				else if (event.key.code == sf::Keyboard::D)
					thirdSpeed = std::max(double(-5), thirdSpeed - 0.1);

				if (secondBefore != secondSpeed)
				{
					secondTray.clear();
					thirdTray.clear();
				}
				if (thirdBefore != thirdSpeed)
				{
					thirdTray.clear();
				}
			}
		}

		window.clear();
		
		move();
		draw();

		window.display();
	}

	return 0;
}

void Pendulum::move()
{
	firstAng += firstSpeed;
	firstBody.setRotation(firstAng);
	firstEnd.setPosition(cos(firstAng * PI / 180) * firstSize.x, sin(firstAng * PI / 180) * firstSize.x);

	moveSecond();

	moveThird();

	fps++;
	if (clock.getElapsedTime().asSeconds() >= 1)
	{
		clock.restart();
		lastFps = fps;
		fps = 0;
	}
}

void Pendulum::draw()
{
	window.draw(background);

	window.draw(firstStart);
	window.draw(firstBody);
	window.draw(firstEnd);

	window.draw(secondStart);
	window.draw(secondBody);
	window.draw(secondEnd);
	if (drawSecond)
		window.draw(&secondTray[0], secondTray.size(), sf::LinesStrip);

	window.draw(thirdStart);
	window.draw(thirdBody);
	window.draw(thirdEnd);
	if (drawThird)
		window.draw(&thirdTray[0], thirdTray.size(), sf::LinesStrip);

	std::string v1str = std::to_string(int(round(firstSpeed * 10)));
	if (abs(round(firstSpeed * 10) / 10) < 1)
	{
		if (round(firstSpeed * 10) / 10 >= 0)
		{
			v1str.insert(v1str.begin(), '.');
			v1str.insert(v1str.begin(), '0');
		}
		else
		{
			v1str.insert(v1str.begin() + 1, '.');
			v1str.insert(v1str.begin() + 1, '0');
		}
	}
	else
	{
		if (round(firstSpeed * 10) / 10 >= 0)
		{
			v1str.insert(v1str.begin() + 1, '.');
		}
		else
		{
			v1str.insert(v1str.begin() + 2, '.');
		}
	}

	std::string v2str = std::to_string(int(round(secondSpeed * 10)));
	if (abs(round(secondSpeed * 10) / 10) < 1)
	{
		if (round(secondSpeed * 10) / 10 >= 0)
		{
			v2str.insert(v2str.begin(), '.');
			v2str.insert(v2str.begin(), '0');
		}
		else
		{
			v2str.insert(v2str.begin() + 1, '.');
			v2str.insert(v2str.begin() + 1, '0');
		}
	}
	else
	{
		if (round(secondSpeed * 10) / 10 >= 0)
		{
			v2str.insert(v2str.begin() + 1, '.');
		}
		else
		{
			v2str.insert(v2str.begin() + 2, '.');
		}
	}

	std::string v3str = std::to_string(int(round(thirdSpeed * 10)));
	if (abs(round(thirdSpeed * 10) / 10) < 1)
	{
		if (round(thirdSpeed * 10) / 10 >= 0)
		{
			v3str.insert(v3str.begin(), '.');
			v3str.insert(v3str.begin(), '0');
		}
		else
		{
			v3str.insert(v3str.begin() + 1, '.');
			v3str.insert(v3str.begin() + 1, '0');
		}
	}
	else
	{
		if (round(thirdSpeed * 10) / 10 >= 0)
		{
			v3str.insert(v3str.begin() + 1, '.');
		}
		else
		{
			v3str.insert(v3str.begin() + 2, '.');
		}
	}

	text.setPosition(-940, -1080 / 2);
	text.setString("V1 = " + v1str + "° / frame (q-a)\n" + 
		"V2 = " + v2str + " * V1 (w-s)\n" +
		"V3 = " + v3str + " * V1 (e-d)");
	window.draw(text);

	text.setPosition(-940, 1080 / 2-80);
	text.setString("ratio = 1.0 : " + v2str + " : " + v3str);
	window.draw(text);

	text.setPosition(490, 1080 / 2 - 80);
	text.setString("frames / sec = " + std::to_string(lastFps));
	window.draw(text);
}

void Pendulum::moveSecond()
{
	secondAng += firstSpeed * secondSpeed;
	actualSecondAng = firstAng + secondAng;
	secondStart.setPosition(firstEnd.getPosition());
	secondBody.setPosition(firstEnd.getPosition());
	secondBody.setRotation(actualSecondAng);
	sf::Vector2f secondLenght(cos(actualSecondAng * PI / 180) * secondSize.x, sin(actualSecondAng * PI / 180) * secondSize.x);
	secondEnd.setPosition(firstEnd.getPosition() + secondLenght);
	sf::Vertex secondVertex(secondEnd.getPosition(), sf::Color::Black);
	secondTray.push_back(secondVertex);
}

void Pendulum::moveThird()
{
	thirdAng += firstSpeed * thirdSpeed;
	actualThirdAng = actualSecondAng + thirdAng;
	thirdStart.setPosition(secondEnd.getPosition());
	thirdBody.setPosition(secondEnd.getPosition());
	thirdBody.setRotation(actualThirdAng);
	sf::Vector2f thirdLenght(cos(actualThirdAng * PI / 180) * thirdSize.x, sin(actualThirdAng * PI / 180) * thirdSize.x);
	thirdEnd.setPosition(secondEnd.getPosition() + thirdLenght);
	sf::Vertex thirdVertex(thirdEnd.getPosition(), sf::Color::Red);
	thirdTray.push_back(thirdVertex);
}

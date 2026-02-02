#include <iostream>
#include <SFML/Graphics.hpp>
#include "transform.hpp"

static sf::Color getFrequencyColor(double magnitude, double maxMagnitude) {
	// 0 ~ tip, 2 = base
	float c = 2.f * std::pow(magnitude / maxMagnitude, 0.3);
	if (c < 1.f)
		return sf::Color(255 * (1 - c), 255 * c, 0);
	else
		return sf::Color(0, 255 * (2 - c), 255 * (c - 1));
}

static void updateRenderers(sf::RenderTexture& bgRenderer, sf::RenderTexture& spectrumRenderer, 
	sf::Shader& bgShader, const std::vector<Point>* spectrum, sf::Vector2u size) 
{
	unsigned int spectrumWidth = size.x / 4;
	sf::Vector2u bgSize = { size.x - spectrumWidth, size.y };
	auto _ = bgRenderer.resize(bgSize);
	bgRenderer.clear();
	bgShader.setUniform("resolution", sf::Vector2f(bgSize));
	sf::RectangleShape quad(sf::Vector2f{ bgSize });
	bgRenderer.draw(quad, &bgShader);
	bgRenderer.display();

	_ = spectrumRenderer.resize({ spectrumWidth, size.y });
	spectrumRenderer.clear(sf::Color(20, 20, 20));
	sf::RectangleShape centerLine(sf::Vector2f(spectrumWidth / 30.f, size.y - 40));
	centerLine.setOrigin({ centerLine.getSize().x / 2.f, 0 });
	centerLine.setPosition({ spectrumWidth / 2.f, 20.f });
	centerLine.setFillColor(sf::Color(100, 100, 100));
	spectrumRenderer.draw(centerLine);

	//no spectrum bars to draw
	if (spectrum == nullptr) {
		spectrumRenderer.display();
		return;
	}

	double maxMagnitude = 0;
	for (int i = 1; i < spectrum->size(); i++)
		maxMagnitude = std::max(maxMagnitude, std::abs((*spectrum)[i]) / spectrum->size());
	float barBase = (centerLine.getSize().y - 10) / (spectrum->size() / 2.f);
	float maxBarHeight = (spectrumWidth - centerLine.getSize().x) / 2.2f;	
	
	auto barSize = [maxBarHeight, maxMagnitude] (double mag) {
		return maxBarHeight * log(1 + mag) / log(1 + maxMagnitude);
		};

	for (int i = 1; i < spectrum->size(); i++) {
		double magnitude = std::abs((*spectrum)[i]) / spectrum->size();
		sf::RectangleShape bar(sf::Vector2f(barSize(magnitude), barBase));
		bar.setFillColor(getFrequencyColor(magnitude, maxMagnitude));
		
		float barX = (spectrumWidth + centerLine.getSize().x) / 2.f;
		float barY = 25.f + (i - 1) * barBase;
		if (i > spectrum->size() / 2.f) {
			barX -= centerLine.getSize().x;
			barY = size.y - 25.f - (i - spectrum->size() / 2.f) * barBase;
			bar.setRotation(sf::degrees(180.f));
		}
		bar.setPosition({ barX, barY });
		spectrumRenderer.draw(bar);
	}

	spectrumRenderer.display();
}

int main()
{
	sf::Shader bgShader;
	if (!bgShader.loadFromFile("bg.frag", sf::Shader::Type::Fragment))
		return -1;

	const float width = sf::VideoMode::getFullscreenModes()[0].size.x * 2 / 3.f;
	sf::Vector2f wSize(width, width * 9.f / 16.f);
	sf::ContextSettings settings;
	settings.antiAliasingLevel = 8;

	sf::RenderWindow w(sf::VideoMode(sf::Vector2u(wSize)), "Fourier-transform", sf::State::Windowed, settings);
	w.setKeyRepeatEnabled(false);
	w.setFramerateLimit(100);

	sf::RenderTexture bgRenderer;
	sf::RenderTexture spectrumRenderer;
	sf::Vector2u lastRenderSize = w.getSize();
	updateRenderers(bgRenderer, spectrumRenderer, bgShader, nullptr, lastRenderSize);

	sf::Vector2f lastMousePos;
	bool isMoving = false, isRunning = false;
	bool isDrawing = false;
	float zoom = 1.0, threshold = 0.05f;
	size_t numFrequencies = 0;
	size_t drawingFrame = size_t(-1);

	std::vector<std::vector<Point>> points;
	Transform* ft = new Transform({});

	std::vector<sf::VertexArray> lines;
	sf::VertexArray traced(sf::PrimitiveType::LineStrip);

	while (w.isOpen()) {
		while (const std::optional event = w.pollEvent()) {
			if (event->is<sf::Event::Closed>())
				w.close();
			//start a line or start moving view
			else if (const auto* button = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (button->button == sf::Mouse::Button::Left) {
					if (drawingFrame == size_t(-1)) {
						lines.push_back(sf::VertexArray(sf::PrimitiveType::LineStrip));
						points.push_back({});

						isDrawing = true;
					}
					else
						isMoving = true;
					lastMousePos = w.mapPixelToCoords(button->position);
				}
			}
			//end a line (discard if too short) or stop moving view
			else if (const auto* button = event->getIf<sf::Event::MouseButtonReleased>()) {
				if (button->button == sf::Mouse::Button::Left) {
					if (isDrawing && points[points.size() - 1].size() < 20) {
						lines.pop_back();
						points.pop_back();
					}
					isDrawing = false, isMoving = false;
				}
			}
			//draw line or move view
			else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>()) {
				if (isDrawing) {
					auto p = w.mapPixelToCoords(mouse->position);
					points[points.size() - 1].push_back({ p.x, p.y });
					lines[lines.size() - 1].append(sf::Vertex(p, sf::Color::White));
				}
				else if (isMoving) {
					sf::View v = w.getView();
					v.move(lastMousePos - w.mapPixelToCoords(mouse->position));
					w.setView(v);

					lastMousePos = w.mapPixelToCoords(mouse->position);
				}
			}
			//change zoom value
			else if (const auto* mouse = event->getIf<sf::Event::MouseWheelScrolled>()) {
				if (!lines.empty())
					zoom /= ((mouse->delta > 0) ? 0.85f : 1.15f);
				zoom = std::clamp(zoom, 0.3f, 5000.f);
			}
			//perform new fourier transform, pause/resume simulation, or reset
			else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
				if (key->code == sf::Keyboard::Key::Space) {
					if (drawingFrame == size_t(-1) && !isDrawing && !points.empty()) {
						isRunning = true;

						std::vector<Point> continuous;
						for (auto& p : points) {
							for (auto& pt : p)
								continuous.push_back(pt);
						}

						auto uniform = Transform::smoothenPoints(continuous);
						delete ft;
						ft = new Transform(uniform);
						updateRenderers(bgRenderer, spectrumRenderer, bgShader, &ft->spectrum_, lastRenderSize);

						for (int i = 0; i < ft->spectrum_.size(); i++)
							std::cout << "f " << i << " -> mag: " << std::abs(ft->spectrum_[i]) / ft->spectrum_.size()
							<< "\tphase: " << std::arg(ft->spectrum_[i]) << "\n";

						std::cout << "raw points number: " << continuous.size() << "\n";
						std::cout << "uniform points number: " << uniform.size() << "\n";

						for (size_t i = 0; i < ft->orderedSpectrum_.size(); i++)
							numFrequencies += (ft->orderedSpectrum_[i].magnitude > threshold);
						std::cout << "frequencies with magnitude > " << threshold << ": " << numFrequencies - 1 << "\n";
					}
					else if (drawingFrame != size_t(-1))
						isRunning = !isRunning;
				}
				else if (key->code == sf::Keyboard::Key::R && !isDrawing) {
					isRunning = false;
					drawingFrame = size_t(-1);
					numFrequencies = 0;

					points.clear();
					lines.clear();
					traced.clear();

					delete ft;
					ft = new Transform({});
					updateRenderers(bgRenderer, spectrumRenderer, bgShader, nullptr, lastRenderSize);

					zoom = 1.f;
					sf::View v = w.getView();
					v.setSize(sf::Vector2f(w.getSize()));
					w.setView(v);
				}
			}
		}
		
		//handle zoom and resize
		sf::View v = w.getView();
		v.setSize(sf::Vector2f(w.getSize()) / zoom);
		w.setView(v);
		if (lastRenderSize != w.getSize()) {
			lastRenderSize = w.getSize();
			if (ft->spectrum_.size() > 0)
				updateRenderers(bgRenderer, spectrumRenderer, bgShader, &ft->spectrum_, lastRenderSize);
			else
				updateRenderers(bgRenderer, spectrumRenderer, bgShader, nullptr, lastRenderSize);
		}

		//draw background
		w.clear();
		sf::Sprite bgSprite(bgRenderer.getTexture());
		bgSprite.setPosition(w.mapPixelToCoords({ 0, 0 }));
		bgSprite.scale(sf::Vector2f(1 / zoom, 1 / zoom));
		w.draw(bgSprite);
		
		//draw FT arms
		const float speed = 150 / 1'000.f;
		if (drawingFrame != size_t(-1) || isRunning) {
			if (isRunning)
				drawingFrame++;

			float t = speed * drawingFrame / ft->orderedSpectrum_.size();
			const auto& anchor = ft->orderedSpectrum_[0];
			float x = anchor.magnitude * std::cos(anchor.phase);
			float y = anchor.magnitude * std::sin(anchor.phase);

			for (const auto& f : ft->orderedSpectrum_) {
				if (f.magnitude <= threshold || f.m == 0)
					continue;

				//max frequency at index 1 (after DC component)
				sf::Color color = getFrequencyColor(f.magnitude, ft->orderedSpectrum_[1].magnitude);
				float radius = std::pow(f.magnitude - threshold, 0.7f) / 17.5f;

				sf::CircleShape circle(radius);
				circle.setOrigin({ radius, radius });
				circle.setPosition({ x, y });
				circle.setFillColor(color);
				w.draw(circle);

				float angle = 2 * PI * f.m * t + f.phase;
				sf::RectangleShape body({ float(f.magnitude), 2.f * radius });
				body.setOrigin({ 0, radius });
				body.setRotation(sf::radians(angle));
				body.setPosition({ x, y });
				body.setFillColor(color);
				w.draw(body);

				x += f.magnitude * std::cos(angle);
				y += f.magnitude * std::sin(angle);

				circle.setPosition({ x, y });
				w.draw(circle);
			}

			if (isRunning && (speed * drawingFrame <= ft->orderedSpectrum_.size()))
				traced.append(sf::Vertex(sf::Vector2f(x, y), sf::Color::Red));
		}

		//draw hand-drawn lines
		for (auto& line : lines)
			w.draw(line);
		//draw already-traded path
		w.draw(traced);

		//draw spectrum
		sf::Sprite spectrumSprite(spectrumRenderer.getTexture());
		spectrumSprite.setPosition(w.mapPixelToCoords(sf::Vector2i(w.getSize().x - spectrumSprite.getTextureRect().size.x, 0)));
		spectrumSprite.scale(sf::Vector2f(1 / zoom, 1 / zoom));
		w.draw(spectrumSprite);

		w.display();
	}

	if (ft != nullptr)
		delete ft;

	return 0;
}
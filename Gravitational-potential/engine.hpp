#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include "gravity.hpp"

class Engine3D {
public:
	Engine3D(unsigned int fps);
    sf::RenderWindow& createWindow(sf::Vector2u size);
    
	int handleEvents();
	void renderPotentialField(GravitySimulator& sim);
	void renderBodies(std::vector<Body>& bodies);

private:
	// by default the camera is looking in the positive z direction
	// the positive x axis is to the right, and the positive y axis is down
	struct Camera {
	    Vec3 position = Vec3(0, 0, 0);
	
		//yaw: angle on the xz plane from the z axis towards the x axis
		//positive yaw = looking right
		double yaw = 0.0;
		//pitch: angle on the yz plane from the z axis towards the y axis
		//positive pitch = looking down
		double pitch = 0.0;
	    //in radiants
	    double fov = 0.0;   
	};

	Vec3 transformToCameraSpace(const Vec3& pos) const;

	sf::RenderWindow window_;
	Camera camera_;

	Vec3 lightDirection_;
	sf::Shader sphereShader_;

	sf::Vector2i lastMousePos_;
	bool isFullscreen_ = false;
	unsigned int fps_ = 0;
	sf::Vector2u defaultWindowSize_;
	sf::ContextSettings settings_;

	double f = 0;
};
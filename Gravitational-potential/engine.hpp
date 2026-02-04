#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

constexpr double PI = 3.14159265358979323846;
typedef sf::Vector3<double> Vec3;

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

class Engine3D {
public:
	Engine3D();
    sf::RenderWindow& createWindow(sf::Vector2u size);
    
	void handleEvents();
	void renderPotentialField(std::function<double(double, double)> dist);
    void renderMasses() {};

private:
	Vec3 transformToCameraSpace(const Vec3& pos) const;

	sf::RenderWindow window_;
	Camera camera_;

	sf::Vector2i lastMousePos_;

};
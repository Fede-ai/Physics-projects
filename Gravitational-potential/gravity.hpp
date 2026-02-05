#pragma once
#include <SFML/System.hpp>

constexpr double PI = 3.14159265358979323846;
typedef sf::Vector3<double> Vec3;

struct Body {
	//add body with mass
	Body(Vec3 p, Vec3 v, double m)
		: position(p), velocity(v), mass(m), 
		radius (std::pow(mass, 1 / 3.0))
	{
	}
	//add massless body
	Body(Vec3 p, Vec3 v)
		: position(p), velocity(v)
	{
	}

	Vec3 position;
	Vec3 velocity;
	const double mass = 0;
	const double radius = 1;
};

class GravitySimulator {
public:
	double getPotentialAtPoint(double x, double z) const;
	void step(double dt);

	void addBodies(std::vector<Body>& bodies) {
		for (auto& body : bodies)
			bodies_.push_back(&body);
	}

private:
	void computeAccelerations(std::vector<Vec3>& accels);

	std::vector<Body*> bodies_;

	static constexpr double G = 50;
	static constexpr double potentialScaling = 0.1;
};

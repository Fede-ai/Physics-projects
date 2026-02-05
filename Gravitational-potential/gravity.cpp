#include "gravity.hpp"
#include <cmath>
#include <algorithm>

double GravitySimulator::getPotentialAtPoint(double x, double z) const
{
	double potential = 0.0;
	for (const auto& b : bodies_) {
		//skip massless bodies
		if (b->mass <= 0)
			continue;

		double dx = x - b->position.x;
		double dz = z - b->position.z;
		double r = std::sqrt(dx * dx + dz * dz);

		//potential outside the mass
		if (r >= b->radius)
			potential += G * b->mass / r;
		//potential inside the mass
		else {
			double R = b->radius;
			double rr = r / R;
			potential += G * b->mass * (3.0 - rr * rr) / (2.0 * R);
		}
	}

	return potential * potentialScaling;
}

void GravitySimulator::step(double dt)
{
	//softening to avoid singularities
	constexpr double eps = 1e-6;   

	for (size_t i = 0; i < bodies_.size(); ++i) {
		Vec3 acceleration(0.0, 0.0, 0.0);

		for (size_t j = 0; j < bodies_.size(); ++j) {
			if (i == j || bodies_[j]->mass <= 0)
				continue;

			Vec3 r = bodies_[j]->position - bodies_[i]->position;
			double dist2 = r.x * r.x + r.y * r.y + r.z * r.z + eps;
			double invDist = 1.0 / std::sqrt(dist2);
			acceleration += G * bodies_[j]->mass * r * invDist * invDist * invDist;
		}

		//semi-implicit Euler
		bodies_[i]->velocity += acceleration * dt;
	}
	for (auto& b : bodies_)
		b->position += b->velocity * dt;
}
